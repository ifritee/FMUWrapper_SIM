
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
  txtFMU_er_init        = 'Не прошла инициализация модели';
  txtFMU_er_parsing     = 'Невозможно произвести разбор файла-дескриптора модели';

  {$ELSE}

  txtCom_er_Unzip       = 'Unzip FMU file is failed';
  txtFMU_er_Create      = 'Create FMU model is failed';
  txtFMU_er_init        = 'Model initialize is crashed';
  txtFMU_er_parsing     = 'Parse description file is crashed';

  {$ENDIF}

implementation

end.
