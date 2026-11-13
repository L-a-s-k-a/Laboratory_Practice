import serial
import time

# Настройки порта
ser = serial.Serial('COM12', 115200, timeout=1)
time.sleep(2)  # Ждем инициализации

print("Управление мотором STM32")
print("Команды: S<скорость> (например, S500)")
print("Для выхода введи 'q'")

while True:
    command = input("Введите команду: ")
    if command.lower() == 'q':
        break
    
    # Отправляем команду
    ser.write((command + '\n').encode())
    print(f"Отправлено: {command}")

ser.close()
print("Соединение закрыто")