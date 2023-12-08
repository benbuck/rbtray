# Functions for making the check box behave like radio buttons
Function .onInit
    StrCpy $1 ${sec02}
FunctionEnd

Function .onSelChange
    !insertmacro StartRadioButtons $1
        !insertmacro RadioButton ${sec02}
        !insertmacro RadioButton ${sec03}
        !insertmacro RadioButton ${sec04}
    !insertmacro EndRadioButtons
FunctionEnd
