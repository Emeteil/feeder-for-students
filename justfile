env := "esp32dev"
arduino_fqbn := "esp32:esp32:esp32"

# Сборка через PlatformIO
build:
    pio run -e {{env}}

# Прошивка платы через PlatformIO (USB, esptool)
upload:
    pio run -e {{env}} -t upload

# Монитор порта (вывод Serial.print и т.п.), выход - Ctrl+C
monitor:
    pio device monitor -b 115200

# Размер прошивки (сколько заняло Flash/RAM)
size:
    pio run -e {{env}} -t size

# Очистить файлы сборки PlatformIO
clean:
    pio run -e {{env}} -t clean

# Сборка тем же способом, что и в Arduino IDE (через arduino-cli).
# CI это НЕ проверяет (см. README, раздел CI) - если меняли структуру
# файлов/#include/библиотеки, прогоните эту команду сами перед PR
arduino-build:
    arduino-cli compile --fqbn {{arduino_fqbn}} .

# Прошивка через arduino-cli. port: см. `just arduino-ports`
arduino-upload port:
    arduino-cli upload -p {{port}} --fqbn {{arduino_fqbn}} .

# Список доступных портов (чтобы узнать port для arduino-upload)
arduino-ports:
    arduino-cli board list
