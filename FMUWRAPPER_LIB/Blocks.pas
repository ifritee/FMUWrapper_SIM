
 //**************************************************************************//
 // Данный исходный код является составной частью системы МВТУ-4             //
 // Программист:        Никишин Е.В.                                        //
 //**************************************************************************//


unit Blocks;

 //***************************************************************************//
 //         Блок для взаимодействия с FMU моделью                             //
 //***************************************************************************//

interface

uses
     Classes
   , DataTypes
   , abstract_im_interface
   , InterfaceUnit
   , RunObjts
   , System.Zip
   , IOUtils
   ;

type

  //Блок - встроенный клиент OPC
  //по умолчанию настроен на тестовый сервер Matricon
  TFMUDataBlock = class(TRunObject)
  public

    constructor  Create(Owner: TObject);override;
    destructor   Destroy;override;
    function     InfoFunc(Action: integer;aParameter: NativeInt):NativeInt;override;
    function     RunFunc(var at,h : RealType;Action:Integer):NativeInt;override;
    function     GetParamID(const ParamName:string;var DataType:TDataType;var IsConst: boolean):NativeInt;override;
    procedure    RestartSave(Stream: TStream);override;
    function     RestartLoad(Stream: TStream;Count: integer;const TimeShift:double):boolean;override;
    procedure    EditFunc(Props:TList;
                          SetPortCount:TSetPortCount;
                          SetCondPortCount:TSetCondPortCount;
                          ExecutePropScript:TExecutePropScript
                          );override;


  strict private

    //*****  Редактируемые параметры блока *****
    m_nextTime: double; /// Целевое время
    m_modelIndex: Integer; /// Индекс модели
    m_fileName: String; /// Имя файла с моделью FMU
    m_isRecalc: Boolean; /// Состояние пересборки модели
    m_saveRecalc: Boolean; /// Сохраненное состояние пересборки модели
    m_modelingTime: Double; /// Время моделирования
    m_modelingStep: Double; /// Шаг моделирования

    //***** Местные свойства ********
    m_outPorts, m_inPorts : Integer;
    m_outPortsType, m_inPortsType : Array of Integer;
    m_outPortsName, m_inPortsName : Array of AnsiString;

    //***** Локальные методы *******
    // Распаковка модели
    function unzipModel(var error : String) : Boolean;
    // Создание новой модели
    function createModel(var error : String) : Boolean;
    // Считывает параметры из модели
    function readModelVars() : Boolean;
    // Записывает параметры в модель
    function writeModelVars() : Boolean;

  const
    // Тип создаваемых портов (под математическую связь)
    PortType = 0;
    TEMP_PATH = 'tmp';

  end;

implementation

uses
     Info
   , SysUtils
   , fmu_texts
   , IntArrays
   , RealArrays
   , DataObjts
   , FMUWrapperLibrary
   ;

//*****  Внешние методы *****

constructor  TFMUDataBlock.Create;
begin
  inherited;
  m_nextTime := 0;
  m_modelIndex := -1;
  m_isRecalc := False;
  m_saveRecalc := m_isRecalc;
end;

destructor   TFMUDataBlock.Destroy;
begin
  inherited;
  if m_modelIndex <> -1 then begin
    freeFMU(m_modelIndex);
  end;
end;

function     TFMUDataBlock.InfoFunc(Action: integer;aParameter: NativeInt):NativeInt;
var
  elementInfo : TElementInfo;
  basePort : TBasePort;
  I, outputCounter : Integer;
begin
  Result := t_none;
  case Action of
    i_GetBlockType: begin
      Result := t_fun;
    end;
    i_GetInit: begin
      //По умолчанию блок - приоритетный, т.к. он полностью асинхронный
      Result := t_src;

    end;
    i_GetCount: begin
      outputCounter := 0;
      DllInfo.Main.GetElementInfo(VisualObject, elementInfo);
      for I := 0 to elementInfo.GetPortCount - 1 do begin
        basePort := elementInfo.GetPort(I);
        if basePort.Mode = pmOutput then begin
          // Тут нужно обработать задание размера выходного порта
          cY[outputCounter] := 1;
          inc(outputCounter);
        end;
      end;
    end;
  else
    Result:=inherited InfoFunc(Action,aParameter);
  end;
end;

function TFMUDataBlock.RunFunc;
var
  retCode, I, ttype : Integer;
  buffer : Array [0..256] of AnsiChar;
  error : String;
begin
 Result := r_Success;

  case Action of
    f_Stop: begin
      if m_modelIndex >= 0 then begin
        freeFMU(m_modelIndex);
        m_modelIndex := -1;
      end;
    end;
    f_InitObjects: begin
      if NOT DirectoryExists(TEMP_PATH) then  begin // Для ускорения
        if (unzipModel(error) = False ) then begin
          ErrorEvent(error, msError, VisualObject);
          Result := r_Fail;
          Exit;
        end;
      end;
    end;
    f_InitState: begin
      if (createModel(error) = False ) then begin
        ErrorEvent(error, msError, VisualObject);
        Result := r_Fail;
        Exit;
      end;

      m_outPorts := outputsQty(m_modelIndex);
      m_inPorts := inputsQty(m_modelIndex);
      SetLength(m_outPortsType, m_outPorts);
      SetLength(m_inPortsType, m_inPorts);
      SetLength(m_outPortsName, m_outPorts);
      SetLength(m_inPortsName, m_inPorts);
      for I := 0 to m_outPorts - 1 do begin
        retCode := outputVar(m_modelIndex, I, buffer, 256, ttype);
        if (retCode = 0) then begin
          m_outPortsType[I] := ttype;
          m_outPortsName[I] := buffer;
        end;
      end;
      for I := 0 to m_inPorts - 1 do begin
      retCode := inputVar(m_modelIndex, I, buffer, 256, ttype);
        if (retCode = 0) then begin
          m_inPortsType[I] := ttype;
          m_inPortsName[I] := buffer;
        end;
      end;
    end;
    f_GoodStep: begin
      if m_modelIndex < 0 then begin // Проверка на существования модели
        ErrorEvent(txtFMU_er_Create, msError, VisualObject);
        Result := r_Fail;
        Exit;
      end;
      if (writeModelVars() = False) then begin
        ErrorEvent(txtFMU_er_Write, msError, VisualObject);
        Result := r_Fail;
        Exit;
      end;

      step(m_modelIndex);
      if readModelVars() = False then begin
        ErrorEvent(txtFMU_er_type, msError, VisualObject);
        Result := r_Fail;
        Exit;
      end;

    end;
  end;
end;

function TFMUDataBlock.GetParamID(const ParamName:string; var DataType:TDataType; var IsConst: boolean) : NativeInt;
begin
  Result:=inherited GetParamId(ParamName,DataType,IsConst);
  if Result = -1 then begin
    if StrEqu(ParamName,'file_name') then begin
      Result:=NativeInt(@m_fileName);
      DataType:=dtFileName;
      Exit;
    end else if StrEqu(ParamName,'recalculate') then begin
      Result:=NativeInt(@m_isRecalc);
      DataType:=dtBool;
      Exit;
    end else if StrEqu(ParamName,'modeling_time') then begin
      Result:=NativeInt(@m_modelingTime);
      DataType:=dtDouble;
      Exit;
    end else if StrEqu(ParamName,'step') then begin
      Result:=NativeInt(@m_modelingStep);
      DataType:=dtDouble;
      Exit;
    end
  end;
end;

procedure TFMUDataBlock.RestartSave(Stream: TStream);
begin
  inherited;
  Stream.Write(m_nextTime,SizeOf(double));
end;

function     TFMUDataBlock.RestartLoad(Stream: TStream;Count: integer;const TimeShift:double):boolean;
begin
  Result:=inherited RestartLoad(Stream,Count,TimeShift);
  Stream.Read(m_nextTime,SizeOf(double));
  m_nextTime:=m_nextTime - TimeShift;
end;


procedure TFMUDataBlock.EditFunc;
var
  locError : String;
  outPorts, inPorts, retCode, I, ttype : Integer;
  ExitWithInfo: TFunc<String, Integer>;   /// Информация при выходе с ошибкой
  buffer : Array [0..256] of AnsiChar;
begin
  //------------------------------------
  ExitWithInfo := function(error: String) : Integer
  begin
    Result:= 0;
    ErrorEvent(error, msError, VisualObject);
    freeFMU(m_modelIndex);
    m_modelIndex := -1;
  end;
  //------------------------------------
  if (m_isRecalc <> m_saveRecalc) AND (FileExists(m_fileName)) then begin
    m_saveRecalc := m_isRecalc;
    // 0. Удалим временную папку и очистим модель
    if m_modelIndex <> -1 then begin
      freeFMU(m_modelIndex);
      m_modelIndex := -1;
    end;
    if unzipModel(locError) = False then begin
      ExitWithInfo(locError);
    end;

    if createModel(locError) = False then begin
      ExitWithInfo(locError);
    end;
    //----- Очистим все порты -----
    outPorts := outputsQty(m_modelIndex);
    inPorts := inputsQty(m_modelIndex);
    SetPortCount(VisualObject, 0, pmOutput, 0, sdRight);
    SetPortCount(VisualObject, 0, pmInput, 0, sdLeft);
    for I := 0 to outPorts - 1 do begin
      retCode := outputVar(m_modelIndex, I, buffer, 256, ttype);
      if (retCode = 0) then begin
        SetCondPortCount(VisualObject, 1,  pmOutput,  portType, sdRight, String(buffer));
      end;
    end;
    for I := 0 to inPorts - 1 do begin
      retCode := inputVar(m_modelIndex, I, buffer, 256, ttype);
      if (retCode = 0) then begin
        SetCondPortCount(VisualObject, 1,  pmInput,  portType, sdLeft, String(buffer));
      end;
    end;
    freeFMU(m_modelIndex);
    m_modelIndex := -1;
  end;
end;

  //*****
  function TFMUDataBlock.unzipModel(var error : String) : Boolean;
  var
  zipFile : TZipFile;
  begin
    Result := True;
    if DirectoryExists(TEMP_PATH) then  begin
      TDirectory.Delete(TEMP_PATH, True);
    end;
    // 1. Распаковываем файл во временную папку
    zipFile :=  TZipFile.Create;
    try
      zipFile.Open(m_fileName, zmRead);
      zipFile.ExtractAll(TEMP_PATH);
      zipFile.Close;
    except
      zipFile.Free;
      error := txtCom_er_Unzip;
      Result := False;
      Exit;
    end;
    zipFile.Free;
  end;

  function TFMUDataBlock.createModel(var error : String) : Boolean;
  begin
    Result := True;
    // 3. Создаем модель
    m_modelIndex := createFMU(TEMP_PATH);
    if (m_modelIndex < 0 ) then begin
      error := txtFMU_er_Create;
      Result := False;
      Exit;
    end;
    // 4. Разбираем файл дескриптора
    if (parsing(m_modelIndex) = CODE_FAILED) then begin
      error := txtFMU_er_parsing;
      Result := False;
      Exit;
    end;
    if (initialize(m_modelIndex, m_modelingTime, m_modelingStep) = CODE_FAILED) then begin
      error := txtFMU_er_init;
      Result := False;
      Exit;
    end;
  end;

  function TFMUDataBlock.readModelVars() : Boolean;
  var
    I : Integer;
  begin
    Result := True;
    for I := 0 to cY.Count - 1 do begin
      if (m_outPortsType[I] = FMU_REAL) then begin // Для DOUBLE
        Y[I].Arr^[0] := getDouble(m_modelIndex, PAnsiChar(m_outPortsName[I]));
      end else if (m_outPortsType[I] = FMU_INTEGER) OR (m_outPortsType[I] = FMU_ENUMERATION) then begin
        Y[I].Arr^[0] := getInt(m_modelIndex, PAnsiChar(m_outPortsName[I]));
      end else if (m_outPortsType[I] = FMU_BOOLEAN) then begin
        Y[I].Arr^[0] := Integer(getBool(m_modelIndex, PAnsiChar(m_outPortsName[I])));
      end else if (m_outPortsType[I] = FMU_STRING) then begin
        // Тут хз
      end else begin
        Result := False;
      end;
    end;
  end;

  function TFMUDataBlock.writeModelVars() : Boolean;
  var
    I : Integer;
    value : Real;
  begin
    Result := True;
    for I := 0 to cU.Count - 1 do begin
      value := U[I].Arr^[0];
      if (m_inPortsType[I] = FMU_REAL) then begin
        setDouble(m_modelIndex, PAnsiChar(m_inPortsName[I]), value);
      end else if (m_inPortsType[I] = FMU_INTEGER) OR (m_inPortsType[I] = FMU_ENUMERATION) then begin
        setInt(m_modelIndex, PAnsiChar(m_inPortsName[I]), Trunc(value));
      end else if (m_inPortsType[I] = FMU_BOOLEAN) then begin
        setBool(m_modelIndex, PAnsiChar(m_inPortsName[I]), value > 0.0);
      end  else if (m_inPortsType[I] = FMU_STRING) then begin
        // Тут хз
      end else begin
        Result := False;
      end;
    end;
  end;
end.


