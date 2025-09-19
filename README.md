- [Описание на русском](#ru_description)  
      - [Список поддерживаемых контроллеров](#mk_list_ru)  
      - [Описание файлов](#file_ru_description)
***
- [Description in English](#en_description)  
      - [List of supported controllers](#mk_list_en)  
      - [File description](#file_en_description)  
<br/>

## <a name="ru_description"></a> Лабораторный практикум по программированию микроконтроллеров
Здесь будет находится стартовый проект для программирования лабораторного стенда Nucleo-144 на базе микроконтроллера STM32F429ZIT6. Помимо этого, данный проект позволяет программировать микроконтроллеры других серий.
#### <a name="mk_list_ru"></a> Список поддерживаемых контроллеров
- STM32F446RE
- STM32F429ZI
- STM32F411VE/CE
- STM32F407VE
- STM32F103C8
- STM32F103C6

#### <a name="file_ru_description"></a> Описание файлов содержащихся в проекте  

**Makefile** - не имеет расширения, является инструкцией для сборщика/компилятора. В нём уже прописано всё необходимое для работы с микроконтроллерами представленными выше, за исключением исполняемых файлов проекта. 

> - Для каждого нового исполняемого файла в проекте потребуется прописывать дирректорию, в которойй он нахоится. Это можно сделать в блоке **C sources** в переменной *C_SOURCES*.
> - Строка 16 указывает на проект, который собирается (*TARGET*). По умолчанию поле заполняется значением "*STM32F429ZI*". Чтобы переключить сборку под другую серию микроконтроллера необходимо заполнить это поле соответствующим названием из [списка поддерживаемых контроллеров](#mk_list_ru).
> - Строка 31 указывает на директорию, в которую будет собираться проект (*BUILD_DIR*). По умолчанию имеет значение "*build*".
> - В строке 254 прописывается инструкция для загрузки исполняемых файлов в память микроконтроллера с помощью *openOCD*  
> Загрузка осуществляется с помощью следующей инструкции:  
> **flash: all**  
> **openocd -f interface/stlink.cfg -f target/$(*TRGT_CFG*).cfg -c "program $(*BUILD_DIR*)/$(*TARGET*).elf verify reset exit"**

**NAME.svd** - для каждого устройства уникален, находится в папках с устройствами, имеет расширение .svd, необходим для просмотра памяти устройства через VSCode.

**startup_stm32fYYxx.s** - имеет расшщирение .s, является ассемблерным фалом, в котором прописаны все возможные вектора прерываний для микроконтроллеров приведенных выше. Он необходим для отслеживания прерываний и вызова их обработчиков.  

**CMSIS/Include** - папка содержащая заголовочные файлы, необходимые для корректной работы ядра микроконтролла.

**CMSIS/Devices** - содержит папки, которые имеют имена соответствующие названиям серии микроконтроллеров. В свою очередь эти директории подразделяются на **Inc** и **Src**.<br/> 
> - В **Src** находятся исполняемые файлы, один из которых описан ннже **system_stm32fYxx.c**.<br/>
> - В **Inc** находятся загаловочные файлы, необходимые для работы с устройствами входящими в состав микроконтроллера (*Периферией*). ***В этих файлах прописываются все директивы препроцессоров, необходимые для работы с микроконтроллерами***
<br/>

**system_stm32fYxx.c** - файл с описанием настроек системы. В данном файле находится описание двух функций SystemInit() и SystemCoreClockUpdate(), котрые необходимы для корректной настройки тактирования ядра и для аппаратного вычисления чисел с плавающей точкой (*только для контроллеров, в которых присутствует аппаратный FPU*).

## <a name="en_description"></a> Microcontroller Programming Lab Workshop
Here will be the starting project for programming the Nucleo-144 lab bench based on the STM32F429ZIT6 microcontroller. In addition, this project allows you to program microcontrollers of other series.

#### <a name="mk_list_en"></a> List of supported controllers
- STM32F446RE
- STM32F429ZI
- STM32F411VE/CE
- STM32F407VE
- STM32F103C8
- STM32F103C6

#### <a name="file_en_description"></a> Description of files contained in the project     
**Makefile** - has no extension, it is an instruction for the builder/compiler. It already contains everything you need to work with the microcontrollers presented above, with the exception of project executables. 

> - For each new executable file in the project, you will need to specify the directory in which it is stored. This can be done in the **C sources** block in the *C_SOURCES* variable.
> - Line 16 indicates the project that is being built (*TARGET*). By default, the field is filled with the value "*STM32F429ZI*". To switch the assembly to a different microcontroller series, fill in this field with the appropriate name from the [list of supported controllers](#mk_list_en).
> - Line 31 indicates the directory where the project will be built (*BUILD_DIR*). The default value is "*build*".
> - Line 228 contains instructions for loading executable files into the microcontroller's memory using *OpenOCD*  
> The download is performed using the following instructions:  
> **flash: all**  
> **openocd -f interface/stlink.cfg -f target/$(*TRGT_CFG*).cfg -c "program $(*BUILD_DIR*)/$(*TARGET*).elf verify reset exit"**

**NAME.svd** - is unique for each device, located in device folders, has the .svd extension, and is required to view device memory via VSCode.

**startup_stm32fYYxx.s** - has the extension .s, is an assembler file that contains all possible interrupt vectors for the STM32F429ZI microcontroller. It is necessary for tracking interrupts and calling their handlers.

**CMSIS/Include** - is a folder containing header files necessary for the correct operation of the microcontroller core.

**CMSIS/Devices** - contains folders that have names corresponding to the names of the microcontroller series. In turn, these directories are divided into **Inc** and **Src**.<br/> 
> - The **Src** contains executable files, one of which is described below **system_stm32fYxx.c**.<br/>
> - The **Inc** contains the header files necessary for working with the devices included in the microcontroller (*Peripherals*). ***These files contain all the preprocessor directives needed to work with microcontrollers***
<br/>

**system_stm32f4xx.c** - a file describing system settings. This file contains a description of two functions SystemInit() and SystemCoreClockUpdate(), which are necessary for the correct setting of the core clock and for the hardware calculation of floating-point numbers (*only for controllers that have a hardware FPU*).