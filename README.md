# my_device - Linux character device driver

Драйвер символьного устройства (уровень Light): операции read, write, ioctl, регистрация в /dev, /proc и /sys

Буфер устройства - 4 КиБ, режим `mode=1` переводит текст в верхний регистр

## Окружение
- Ядро: 6.1.130, собрано и оптимизировано по ДЗ №1, `.config` - в `kernel/.config`
- Образ: https://github.com/OTUS-Linux-kernel-dev/Linux-kernel-dev/blob/main/doc/module_1/0x04_QA.md
- Ссылка на репозиторий ДЗ №1: https://github.com/elizachi/HW_01_build_kernel

## Сборка и загрузка
```yaml
make                    # сборка модуля my_device.ko
make load               # загрузка (sudo insmod)
make unload             # выгрузка (sudo rmmod)
make reload             # перезагрузка модуля
make tests              # сборка пользовательских тестов
make test               # запуск тестов
make clean
```

После загрузки появляются: `/dev/my_device`, `/proc/my_device`, `/sys/class/my_device/my_device/`

## Использование
### read / write
```bash
echo "hello" | sudo tee /dev/my_device
sudo cat /dev/my_device
```
Запись с начала файла заменяет содержимое, при этом размер **ограничен 4096 байтами** (при переполнении write возвращает `ENOSPC`).

### ioctl (заголовок my_device.h)
| Команда | Аргумент | Действие |
|---|---|---|
| MYDEV_IOC_CLEAR | - | очистить буфер |
| MYDEV_IOC_GET_SIZE | int * | размер буфера |
| MYDEV_IOC_GET_LEN | int * | длина данных |
| MYDEV_IOC_SET_MODE | int * (0/1) | режим записи |
| MYDEV_IOC_GET_MODE | int * | текущий режим |

Запись
```bash
int mode = 1;
int fd = open("/dev/my_device", O_RDWR);
ioctl(fd, MYDEV_IOC_SET_MODE, &mode);
```
Готовый пример: `test/test_ioctl.c`

### /proc/my_device (только чтение)
Выводит: имя, `major/minor`, размер буфера, длину данных, режим, число `read/write`
```bash
cat /proc/my_device
```

### /sys/class/my_device/my_device/
| Файл | Доступ | Описание |
|---|---|---|
| size | ro | размер буфера |
| len | ro | длина данных |
| mode | rw | режим записи (0/1) |

Запись
```bash
cat /sys/class/my_device/my_device/len
echo 1 | sudo tee /sys/class/my_device/my_device/mode
```

## Тестирование
`make test` запускает `test/test_rw` (запись/чтение) и `test/test_ioctl` (управление параметрами через ioctl)

Состояние можно посмотреть в
`/proc/my_device` и `/sys/class/my_device/my_device/`

## Структура проекта
```yaml
├── kernel
│   └── .config
├── Makefile
├── md_fops.c
├── md_internal.h
├── md_ioctl.c
├── md_main.c
├── md_proc.c
├── md_sysfs.c
├── my_device.h
├── README.md
└── test
    ├── test_ioctl.c
    └── test_rw.c
```
**Функционал:**
- `md_main.c` - `init/exit` и регистрация;
- `md_fops.c` - `open/read/write`;
- `md_ioctl.c` - `ioctl`;
- `md_proc.c` - `/proc`;
- `md_sysfs.c` - `/sys`;
- `my_device.h` - общие определения `ioctl`.
