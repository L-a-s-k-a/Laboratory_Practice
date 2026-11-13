param(
    [ValidateSet('all', 'registers', 'macros', 'cmsis')]
    [string]$Variant = 'all',

    [string]$Template = $PSScriptRoot
)

$ErrorActionPreference = 'Stop'

function Invoke-Checked {
    param([string]$Program, [string[]]$Arguments)

    & $Program @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$Program exited with code $LASTEXITCODE"
    }
}

$Template = (Resolve-Path -LiteralPath $Template).Path
$device = Join-Path $Template 'STMDevices/STM32F407xx'
$startup = Join-Path $device 'startup_stm32f407xx.s'
$linker = 'STMDevices/STM32F407xx/STM32F407VGTx_FLASH.ld'
$family = Join-Path $Template 'CMSIS/Devices/STM32F4xx'
$variants = if ($Variant -eq 'all') { 'registers', 'macros', 'cmsis' } else { $Variant }

if (!(Test-Path -LiteralPath $startup -PathType Leaf)) {
    throw "Missing STM32F407 startup file: $startup"
}

Push-Location -LiteralPath $PSScriptRoot
try {
    foreach ($name in $variants) {
        $output = "build/$name"
        New-Item -ItemType Directory -Force -Path $output | Out-Null
        $flags = @('-mcpu=cortex-m4', '-mthumb', '-mfloat-abi=soft', '-g3', '-Og',
                   '-Wall', '-Wextra', '-ffunction-sections', '-fdata-sections',
                   "-ICore/Inc/$name")
        $system = 'Core/Src/system_reset.c'
        if ($name -eq 'cmsis') {
            $flags += @('-DSTM32F407xx', "-I$Template/CMSIS/Include",
                        "-I$family/Inc", "-I$family/Inc/STM32F407xx")
            $system = Join-Path $family 'Src/system_stm32f4xx.c'
        }

        $elf = "$output/lab1.elf"
        $inputs = @("Core/Src/$name/main.c", "Core/Src/$name/init.c", $system, $startup)
        $link = @('--specs=nano.specs', '--specs=nosys.specs', "-T$linker",
                  "-Wl,--gc-sections,-Map=$output/lab1.map", '-o', $elf)
        Invoke-Checked 'arm-none-eabi-gcc' ($flags + $inputs + $link)
        Invoke-Checked 'arm-none-eabi-objcopy' @('-O', 'ihex', $elf, "$output/lab1.hex")
        Invoke-Checked 'arm-none-eabi-objcopy' @('-O', 'binary', $elf, "$output/lab1.bin")
        Invoke-Checked 'arm-none-eabi-size' @($elf)
    }
}
finally {
    Pop-Location
}
