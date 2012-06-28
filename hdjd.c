#include <stdio.h>
#include <stdint.h>
#include <libusb.h>

/*
 * Some things I use for debugging 
 */
#ifdef NODUMP
#define DUMPf(fmt, args...)
#else
#define DUMPf(fmt, args...) fprintf(stderr, "%s:%s:%d " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ##args)
#endif
#define DUMP() DUMPf("")
#define DUMP_d(v) DUMPf("%s = %d", #v, v)
#define DUMP_x(v) DUMPf("%s = 0x%x", #v, v)
#define DUMP_s(v) DUMPf("%s = %s", #v, v)
#define DUMP_c(v) DUMPf("%s = '%c' (0x%02x)", #v, v, v)
#define DUMP_p(v) DUMPf("%s = %p", #v, v)


int
main(int argc, char **argv)
{
    struct libusb_device_handle *handle;
    int             ret;

    if (libusb_init(NULL) < 0) {
        return 69;
    }

    handle = libusb_open_device_with_vid_pid(NULL, 0x06f8, 0xb105);     // MP3e2
    if (!handle) {
        printf("Couldn't find an MP3e2\n");
        return 69;
    }

    /* Make Deck A cue button flash. */
    {
        int transferred;

        uint8_t cmd[] = { 0x90, 0x3e, 0x7f };
        libusb_bulk_transfer(handle, 0x03, cmd, 3, &transferred, 0);
    }

    while (1) {
        uint8_t         data[80];
        int             transferred;
        int             i;

        if ((ret = libusb_bulk_transfer(handle, 0x82,
                                        data, sizeof data,
                                        &transferred, 0))) {
            break;
        }

        for (i = 0; i < transferred; i += 1) {
            printf("%02x ", data[i]);
        }
        printf("\n");

        /* Cram it right back out */

        if ((ret = libusb_bulk_transfer(handle, 0x03,
                                        data, transferred,
                                        &transferred, 0))) {
            break;
        }
    }

    if (ret < 0) {
        printf("ERROR: %s\n", libusb_error_name(ret));
    }

    libusb_exit(NULL);

    return 0;
}