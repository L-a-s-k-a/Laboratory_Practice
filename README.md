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
- STM32F429ZI
- STM32F411VE
- STM32F103C8
- STM32F103C6

#### <a name="file_ru_description"></a> Описание файлов содержащихся в проекте  

**Makefile** - не имеет расширения, является инструкцией для сборщика/компилятора. В нём уже прописано всё необходимое для работы с микроконтроллерами представленными выше, за исключением исполняемых файлов проекта. 

> - Для каждого нового исполняемого файла в проекте потребуется прописывать дирректорию, в которойй он нахоится. Это можно сделать в блоке **C sources** в переменной *C_SOURCES*.
> - Строка 16 указывает на проект, который собирается (*TARGET*). По умолчанию поле заполняется значением "*STM32F429ZI*". Чтобы переключить сборку под другую серию микроконтроллера необходимо заполнить это поле соответствующим названием из [списка поддерживаемых контроллеров](#mk_list_ru).
><br/> - Строка 31 указывает на директорию, в которую будет собираться проект (*BUILD_DIR*). По умолчанию имеет значение "*build*".
><br/> - В строке 228 прописывается инструкция для загрузки исполняемых файлов в память микроконтроллера с помощью *openOCD*  
> Запись осуществляется с помощью следующей инструкции:
><br/> **flash: all**
><br/> **openocd -f interface/stlink.cfg -f target/$(*TRGT_CFG*).cfg -c "program $(*BUILD_DIR*)/$(*TARGET*).elf verify reset exit"**

**NAME.svd** - для каждого устройства уникален, находится в папках с устройствами, имеет расширение .svd, необходим для просмотра памяти устройства через VSCode.

**startup_stm32fYYxx.s** - имеет расшщирение .s, является ассемблерным фалом, в котором прописаны все возможные вектора прерываний для микроконтроллеров приведенных выше. Он необходим для отслеживания прерываний и вызова их обработчиков.  

**CMSIS/Include** - папка содержащая заголовочные файлы, необходимые для корректной работы ядра микроконтролла.

**CMSIS/Devices** - содержит папки, которые имеют имена соответствующие названиям серии микроконтроллеров. В свою очередь эти директории подразделяются на **Inc** и **Src**.<br/> 
>В **Src** находятся исполняемые файлы, один из которых описан ннже **system_stm32fYxx.c**.<br/>
>В **Inc** находятся загаловочные файлы, необходимые для работы с устройствами входящими в состав микроконтроллера (*Периферией*). ***В этих файлах прописываются все, необходимые для работы с микроконтроллерами, определеители***
<br/>

**system_stm32fYxx.c** - файл с описанием настроек системы. В данном файле находится описание двух функций SystemInit() и SystemCoreClockUpdate(), котрые необходимы для корректной настройки тактирования ядра и для аппаратного вычисления чисел с плавающей точкой (*только для контроллеров, в которых присутствует аппаратный FPU*).

## <a name="en_description"></a> Microcontroller Programming Lab Workshop
Here will be the starting project for programming the Nucleo-144 lab bench based on the STM32F429ZIT6 microcontroller. In addition, this project allows you to program microcontrollers of other series.

#### <a name="file_ru_description"></a> List of supported controllers
- STM32F429ZI
- STM32F411VE
- STM32F103C8
- STM32F103C6

#### <a name="file_en_description"></a> Description of files contained in the project   
**STM32F429ZI.svd** - has extension .svd, it is necessary for viewing the device memory via VSCode.  
**Makefile** - has no extension, it is an instruction for the builder/compiler. It already contains everything necessary for working with the STM32F429ZI microcontroller, except for the project executable files.  
> Line 169 is responsible for loading executables into microcontroller memory using *openOCD*  
> It is written using the following instruction:
><br/> **flash: all**
><br/> **openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $(*BUILD_DIR*)/$(*TARGET*).elf verify reset exit"**
><br/> Line 16 points to the project to be built (*TARGET*).
><br/> Line 32 indicates the directory where the project will be built (*BUILD_DIR*).
  
**startup_stm32f429xx.s** - has the extension .s, is an assembler file that contains all possible interrupt vectors for the STM32F429ZI microcontroller. It is necessary for tracking interrupts and calling their handlers.

**system_stm32f4xx.c** - a file describing system settings.