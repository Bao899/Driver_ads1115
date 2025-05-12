
# 📟 Dự án: Linux Kernel Driver cho ADC ADS1115

## 👥 Thông tin nhóm

- Số lượng thành viên: 4
- Thành viên:
  - Trần Văn Kha – MSSV: 22146327
  - Đặng Trần Quốc Huy – MSSV: 22146312
  - Đinh Trương Nhật Bảo – MSSV: 22146267
  - Nguyễn Quốc Bảo – MSSV: 22146270

## 📄 Mô tả đề tài

Dự án viết **driver kernel cho module ADC ADS1115** giao tiếp qua **I2C**. Driver hỗ trợ điều khiển qua giao tiếp **IOCTL**, bao gồm các chức năng:

- Thiết lập kênh đầu vào (channel)
- Cấu hình độ khuếch đại tín hiệu (PGA)
- Cấu hình tốc độ lấy mẫu (SPS)
- Đọc giá trị ADC sau khi chuyển đổi

## 📁 Cấu trúc dự án

```
ads1115_driver/
├── ads1115_driver.c       # Mã nguồn kernel module
├── Makefile               # Makefile để biên dịch module
├── test_ads1115.c         # Ứng dụng người dùng (dùng IOCTL)
└── README.md              # Tài liệu mô tả dự án
```

## 🔧 Mô tả mã nguồn

### 1. Kernel Driver: `ads1115_driver.c`

- Đăng ký thiết bị character `/dev/ads1115`
- Giao tiếp I2C với `i2c_master_send` và `i2c_master_recv`
- Hỗ trợ các IOCTL:
  - `SET_CHANNEL`
  - `SET_PGA`
  - `SET_SPS`
  - `READ_VALUE`
- Sử dụng thanh ghi `CONFIG` để thiết lập chế độ single-shot
- Đọc dữ liệu từ thanh ghi `CONVERSION`

### 2. Ứng dụng người dùng: `test_ads1115.c`

- Mở thiết bị `/dev/ads1115`
- Gửi các IOCTL tương ứng để cấu hình
- Đọc và hiển thị giá trị ADC

### 3. Makefile

```makefile
obj-m += ads1115_driver.o

KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(shell pwd) modules

clean:
	make -C $(KDIR) M=$(shell pwd) clean
```

## ▶️ Hướng dẫn biên dịch và sử dụng

### Bước 1: Biên dịch driver

```bash
make
```

### Bước 2: Nạp module kernel

```bash
sudo insmod ads1115_driver.ko
```


### Bước 3: Kiểm tra thiết bị

```bash
dmesg | tail
ls 
❗ Nếu KHÔNG tồn tại /dev/ads1115, hãy tạo thiết bị I2C thủ công:
echo ads1115 0x48 | sudo tee /sys/bus/i2c/devices/i2c-1/new_device
```

### Bước 4: Chạy ứng dụng người dùng

```bash
gcc -o run test_ads1115.c
./run
```

## ✅ Kết quả demo

Ứng dụng cấu hình ADC để đọc từ **channel 0**, với:

- PGA = ±4.096V
- SPS = 128SPS

**Ví dụ kết quả:**

```
ADC Value: 12345
```

## 🔗 GitHub

[https://github.com/Bao899/Driver_ads1115](https://github.com/Bao899/Driver_ads1115)

## 📝 Đánh giá

- Nhóm đã nắm được cách viết **kernel module** và sử dụng **I2C** trong không gian kernel.
- Sử dụng **IOCTL** giúp giao tiếp đơn giản giữa người dùng và thiết bị.
- Có thể mở rộng để hỗ trợ chế độ continuous hoặc cảnh báo ngưỡng ADC trong tương lai.
