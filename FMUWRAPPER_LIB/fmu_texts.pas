
 //**************************************************************************//
 // Данный исходный код является составной частью системы МВТУ-4             //
 // Программист:        Никишин Е.В.                                        //
 //**************************************************************************//


unit fmu_texts;

interface

const

  {$IFNDEF ENG}

  txtCom_er_Unzip       = 'Ошибка распаковки модели FMU';
  txtFMU_er_Create      = 'При создании модели FMU произошел сбой';

  {$ELSE}

  txtCom_er_Unzip       = 'Unzip FMU file is failed';
  txtFMU_er_Create      = 'Create FMU model is failed';

  {$ENDIF}

implementation

end.
