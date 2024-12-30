- [Описание на русском](#ru_description)  
      - [Описание файлов](#file_ru_description)
***
- [Description in English](#en_description)  
      - [File description](#file_en_description)  
<br/>

## <a name="ru_description"></a> Лабораторный практикум по программированию микроконтроллеров
Здесь будет находится стартовый проект для программирования лабораторного стенда Nucleo-144 на базе микроконтроллера STM32F429ZIT6. 

#### <a name="file_ru_description"></a> Описание файлов содержащихся в проекте  
**STM32F429ZI.svd** -  имеет расширение .svd, необходим для просмотра памяти устройства через VSCode.  
**Makefile** - не имеет расширения, является инструкцией для сборщика/компилятора. В нём уже прописано всё необходимое для работы с микроконтроллером STM32F429ZI, за исключением исполняемых файлов проекта.  
> Строка 169 отвечает за загрузку исполняемых файлов в память микроконтроллера с помощью *openOCD*  
> Запись осуществляется с помощью следующей инструкции:
><br/> **flash: all**
><br/> **openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program $(*BUILD_DIR*)/$(*TARGET*).elf verify reset exit"**
><br/> Строка 16 указывает на проект, который собирается (*TARGET*).
><br/> Строка 32 указывает на директорию, в которую будет собираться проект (*BUILD_DIR*).
  
**startup_stm32f429xx.s** - имеет расшщирение .s, является ассемблерным фалом, в котором прописаны все возможные вектора прерываний для микроконтроллера STM32F429ZI. Он необходим для отслеживания прерываний и вызова их обработчиков.  
***
<br/>

## <a name="en_description"></a> Microcontroller Programming Lab Workshop
Here will be the starting project for programming the Nucleo-144 lab bench based on the STM32F429ZIT6 microcontroller. 

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