
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

    end;
  else
    Result:=inherited InfoFunc(Action,aParameter);
  end;
end;

function TFMUDataBlock.RunFunc;
var
  retCode, I, ttype : Integer;
  buffer : Array [0..256] of AnsiChar;
begin
 Result := r_Success;

  case Action of
    f_Stop: begin

    end;
    f_InitObjects: begin
      if m_modelIndex = -1 then begin
        m_modelIndex := createFMU(TEMP_PATH);
        parsing(m_modelIndex);
        initialize(m_modelIndex, m_modelingTime, m_modelingStep);
      end;
    end;
    f_InitState: begin
      if m_modelIndex < 0 then begin // Проверка на существования модели
        ErrorEvent(txtFMU_er_Create, msError, VisualObject);
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
      step(m_modelIndex);
      for I := 0 to cY.Count - 1 do begin
        if (m_outPortsType[I] = 0) then begin // Для DOUBLE
          Y[I].Arr^[0] := getDouble(m_modelIndex, PAnsiChar(m_outPortsName[I]));
        end;
      end;
    end;
  end;
end;

function     TFMUDataBlock.GetParamID(const ParamName:string; var DataType:TDataType; var IsConst: boolean) : NativeInt;
begin
  Result:=inherited GetParamId(ParamName,DataType,IsConst);
  if Result = -1 then begin
    if StrEqu(ParamName,'file_name') then begin
      Result:=NativeInt(@m_fileName);
      DataType:=dtString;
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
  zipFile : TZipFile;
  outPorts, inPorts, retCode, I, ttype : Integer;
  ExitWithInfo: TFunc<Integer>;   /// Информация при выходе с ошибкой
  buffer : Array [0..256] of AnsiChar;
begin
  //------------------------------------
  ExitWithInfo := function() : Integer
  begin
    Result:= 0;
    ErrorEvent(txtFMU_er_Create, msError, VisualObject);
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
    TDirectory.Delete(TEMP_PATH, True);
    // 1. Распаковываем файл во временную папку
    zipFile :=  TZipFile.Create;
    try
      zipFile.Open(m_fileName, zmRead);
      zipFile.ExtractAll(TEMP_PATH);
      zipFile.Close;
    except
      zipFile.Free;
      ErrorEvent(txtCom_er_Unzip, msError, VisualObject);
      Exit;
    end;
    zipFile.Free;
    // 3. Создаем модель
    m_modelIndex := createFMU(TEMP_PATH);
    if (m_modelIndex < 0 ) then begin
      ExitWithInfo();
      Exit;
    end;
    // 4. Разбираем файл дескриптора
    retCode := parsing(m_modelIndex);
    if (retCode = -1) then begin
      ExitWithInfo();
      Exit;
    end;

    retCode := initialize(m_modelIndex, m_modelingTime, m_modelingStep);
    if (retCode = -1) then begin
      ExitWithInfo();
      Exit;
    end;
    //----- Очистим все порты -----
    outPorts := outputsQty(m_modelIndex);
    inPorts := inputsQty(m_modelIndex);
    SetPortCount(VisualObject, outPorts, pmOutput, 0, sdRight);
    SetPortCount(VisualObject, inPorts, pmInput, 0, sdLeft);
    for I := 0 to outPorts - 1 do begin
      retCode := outputVar(m_modelIndex, I, buffer, 256, ttype);
      if (retCode = 0) then begin
        SetCondPortCount(VisualObject, 1,  pmOutput,  portType, sdRight, buffer);
      end;
    end;
    for I := 0 to inPorts - 1 do begin
      retCode := inputVar(m_modelIndex, I, buffer, 256, ttype);
      if (retCode = 0) then begin
        SetCondPortCount(VisualObject, 1,  pmInput,  portType, sdLeft, buffer);
      end;
    end;
  end;
end;

end.


