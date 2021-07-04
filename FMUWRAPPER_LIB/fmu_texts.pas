
 //**************************************************************************//
 // Данный исходный код является составной частью системы МВТУ-4             //
 // Программист:        Никишин Е.В.                                        //
 //**************************************************************************//


unit fmu_texts;

interface

const

  {$IFNDEF ENG}

  txtCom_er_NoInPrefix       = 'Ошибка инициализации блока: не заданы значения в массиве InPrefix для префикса приёма данных';

  {$ELSE}

  txtCom_er_NoInPrefix       = 'Block initialization error: no values were specified in the InPrefix array for the data receive prefix';

  {$ENDIF}

implementation

end.
