#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Duong dan thiet bi
#define DEVICE_PATH "/dev/ads1115"

// Ma lenh ioctl va cac lenh giao tiep voi driver
#define ADS1115_IOCTL_MAGIC 'a'  // Ma lenh dac biet
#define ADS1115_READ_CHANNEL_0 _IOR(ADS1115_IOCTL_MAGIC, 1, int)  // Lenh doc kenh ADC 0

int main() {
    // Mo thiet bi /dev/ads1115 voi quyen doc/ghi
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Khong the mo thiet bi");
        return 1;
    }

    int value;  // Bien de luu gia tri ADC doc duoc
    while (1) {
        // Thuc hien lenh ioctl de doc gia tri ADC tu kenh 0
        if (ioctl(fd, ADS1115_READ_CHANNEL_0, &value) < 0) {
            perror("Loi doc ADC");
            close(fd);  // Dong file descriptor
            return 1;
        }

        // Chuyen doi gia tri ADC sang dien ap
        double voltage = value * 4.096 / 32768.0;
        // In gia tri ADC va dien ap tuong ung
        printf("ADC: %d, Dien ap: %.4f V\n", value, voltage);

        sleep(1);
    }

    // Dong file descriptor khi ket thuc
    close(fd);
    return 0;
}

