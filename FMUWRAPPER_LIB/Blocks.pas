
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
   , urs232
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

    m_nextTime:    double; /// Целевое время

  const
    // Тип создаваемых портов (под математическую связь)
    PortType = 0;

  end;

implementation

uses
     Info
   , SysUtils
   , fmu_texts
   , IntArrays
   , RealArrays
   , DataObjts
   ;



//*****  Внешние методы *****

constructor  TFMUDataBlock.Create;
begin
  inherited;
  m_nextTime := 0;
end;

destructor   TFMUDataBlock.Destroy;
begin
  inherited;
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

function     TFMUDataBlock.RunFunc;
begin
 Result := r_Success;

  case Action of
    f_Stop: begin

    end;
    f_InitObjects: begin

    end;
    f_InitState: begin

    end;
    f_GoodStep: begin

    end;
  end;
end;

function     TFMUDataBlock.GetParamID(const ParamName:string;var DataType:TDataType;var IsConst: boolean):NativeInt;
begin
  Result:=inherited GetParamId(ParamName,DataType,IsConst);
  if Result = -1 then begin

  end;
end;

procedure    TFMUDataBlock.RestartSave(Stream: TStream);
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


procedure    TFMUDataBlock.EditFunc;
begin

end;

end.


