#include <os.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern unsigned int nl_osid(void);

void _init(void) {}
void _fini(void) {}

#define OSID_CAS_CXII_620333 46u
#define OS_SIGNATURE_CAS_CXII_620333 0x1042AE10u
#define SEND_TO_EVENT_QUEUE_CAS_CXII_620333 0x1042D17Cu
#define SEND_TO_EVENT_QUEUE_WORD0_CAS_CXII_620333 0xE92D4070u
#define SEND_TO_EVENT_QUEUE_WORD1_CAS_CXII_620333 0xE59F4058u

static char qwerty_map_char(char ch) {
    int uppercase = ch >= 'A' && ch <= 'Z';
    char lower = uppercase ? (char)(ch - 'A' + 'a') : ch;
    char mapped = 0;

    switch (lower) {
        case 'a': mapped = 'q'; break;
        case 'b': mapped = 'w'; break;
        case 'c': mapped = 'e'; break;
        case 'd': mapped = 'r'; break;
        case 'e': mapped = 't'; break;
        case 'f': mapped = 'y'; break;
        case 'g': mapped = 'u'; break;
        case 'h': mapped = 'a'; break;
        case 'i': mapped = 's'; break;
        case 'j': mapped = 'd'; break;
        case 'k': mapped = 'f'; break;
        case 'l': mapped = 'g'; break;
        case 'm': mapped = 'h'; break;
        case 'n': mapped = 'j'; break;
        case 'o': mapped = 'k'; break;
        case 'p': mapped = 'l'; break;
        case 'q': mapped = 'z'; break;
        case 'r': mapped = 'x'; break;
        case 's': mapped = 'c'; break;
        case 't': mapped = 'v'; break;
        case 'u': mapped = 'b'; break;
        case 'v': mapped = 'n'; break;
        case 'w': mapped = 'm'; break;
        case 'x': mapped = 'p'; break;
        case 'y': mapped = 'o'; break;
        case 'z': mapped = 'i'; break;
        default: return ch;
    }

    return uppercase ? (char)(mapped - 'a' + 'A') : mapped;
}

static int is_alpha_ascii(unsigned char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

static int pointer_is_plausible_event_buffer(const void *ptr) {
    uintptr_t addr = (uintptr_t)ptr;

    return (addr >= 0x10000000u && addr < 0x14000000u) ||
           (addr >= 0xA0000000u && addr < 0xB0000000u);
}

static void rewrite_key_event(struct s_ns_event *event) {
    unsigned char old_ascii;
    unsigned char new_ascii;
    uint16_t high_bits;

    if (!event || !pointer_is_plausible_event_buffer(event))
        return;

    if (event->type != 0x8 && event->type != 0x10)
        return;

    if (event->modifiers & 0x4)
        return;

    old_ascii = (unsigned char)(event->ascii & 0xFFu);
    if (!is_alpha_ascii(old_ascii))
        return;

    new_ascii = (unsigned char)qwerty_map_char((char)old_ascii);
    high_bits = (uint16_t)(event->ascii & 0xFF00u);
    event->ascii = (uint16_t)(high_bits | new_ascii);

    if ((event->key & 0xFFu) == old_ascii)
        event->key = (event->key & ~0xFFu) | new_ascii;
}

HOOK_DEFINE(qwerty_event_queue_hook) {
    struct s_ns_event *event = (struct s_ns_event *)HOOK_SAVED_REGS(qwerty_event_queue_hook)[0];
    rewrite_key_event(event);
    HOOK_RESTORE_RETURN(qwerty_event_queue_hook);
}

static void print_header(void) {
    printf("Custom QWERTY Keymap\n\n");
    printf("OSID: %u  HW subtype: %u\n", nl_osid(), nl_hwsubtype());
    printf("OS signature @10000020: %08lX\n", (unsigned long)*(volatile uint32_t *)0x10000020u);
    printf("Target: %08lX\n", (unsigned long)SEND_TO_EVENT_QUEUE_CAS_CXII_620333);
    printf("Words: %08lX %08lX\n",
           (unsigned long)*(volatile uint32_t *)SEND_TO_EVENT_QUEUE_CAS_CXII_620333,
           (unsigned long)*(volatile uint32_t *)(SEND_TO_EVENT_QUEUE_CAS_CXII_620333 + 4u));
    printf("\n");
}

static int environment_is_supported(void) {
    volatile uint32_t *target = (volatile uint32_t *)SEND_TO_EVENT_QUEUE_CAS_CXII_620333;

    if (nl_osid() != OSID_CAS_CXII_620333)
        return 0;

    if (*(volatile uint32_t *)0x10000020u != OS_SIGNATURE_CAS_CXII_620333)
        return 0;

    if (target[0] != SEND_TO_EVENT_QUEUE_WORD0_CAS_CXII_620333)
        return 0;

    if (target[1] != SEND_TO_EVENT_QUEUE_WORD1_CAS_CXII_620333)
        return 0;

    return 1;
}

static void wait_for_release(void) {
    wait_no_key_pressed();
    msleep(100);
}

static int wait_menu_key(void) {
    while (1) {
        wait_key_pressed();
        if (isKeyPressed(KEY_NSPIRE_1)) return 1;
        if (isKeyPressed(KEY_NSPIRE_2)) return 2;
        if (isKeyPressed(KEY_NSPIRE_ESC)) return 0;
        wait_for_release();
    }
}

static char scan_alpha_key(void) {
    if (isKeyPressed(KEY_NSPIRE_A)) return 'A';
    if (isKeyPressed(KEY_NSPIRE_B)) return 'B';
    if (isKeyPressed(KEY_NSPIRE_C)) return 'C';
    if (isKeyPressed(KEY_NSPIRE_D)) return 'D';
    if (isKeyPressed(KEY_NSPIRE_E)) return 'E';
    if (isKeyPressed(KEY_NSPIRE_F)) return 'F';
    if (isKeyPressed(KEY_NSPIRE_G)) return 'G';
    if (isKeyPressed(KEY_NSPIRE_H)) return 'H';
    if (isKeyPressed(KEY_NSPIRE_I)) return 'I';
    if (isKeyPressed(KEY_NSPIRE_J)) return 'J';
    if (isKeyPressed(KEY_NSPIRE_K)) return 'K';
    if (isKeyPressed(KEY_NSPIRE_L)) return 'L';
    if (isKeyPressed(KEY_NSPIRE_M)) return 'M';
    if (isKeyPressed(KEY_NSPIRE_N)) return 'N';
    if (isKeyPressed(KEY_NSPIRE_O)) return 'O';
    if (isKeyPressed(KEY_NSPIRE_P)) return 'P';
    if (isKeyPressed(KEY_NSPIRE_Q)) return 'Q';
    if (isKeyPressed(KEY_NSPIRE_R)) return 'R';
    if (isKeyPressed(KEY_NSPIRE_S)) return 'S';
    if (isKeyPressed(KEY_NSPIRE_T)) return 'T';
    if (isKeyPressed(KEY_NSPIRE_U)) return 'U';
    if (isKeyPressed(KEY_NSPIRE_V)) return 'V';
    if (isKeyPressed(KEY_NSPIRE_W)) return 'W';
    if (isKeyPressed(KEY_NSPIRE_X)) return 'X';
    if (isKeyPressed(KEY_NSPIRE_Y)) return 'Y';
    if (isKeyPressed(KEY_NSPIRE_Z)) return 'Z';
    return 0;
}

static void run_probe(void) {
    char pressed;

    printf("\nProbe mode. ESC exits.\n");
    printf("Press alpha keys only.\n");
    printf("source -> mapped\n");
    wait_for_release();

    while (!isKeyPressed(KEY_NSPIRE_ESC)) {
        pressed = scan_alpha_key();
        if (pressed) {
            printf("%c -> %c\n", pressed, qwerty_map_char(pressed));
            wait_for_release();
        }
        msleep(10);
    }

    wait_for_release();
}

static void install_hook(void) {
    printf("\nInstall requested.\n");

    if (!environment_is_supported()) {
        printf("Refusing to install: OS/model/function\n");
        printf("fingerprint is not the verified\n");
        printf("CX II CAS OS 6.2.0.333 target.\n\n");
        printf("Press any key.\n");
        wait_key_pressed();
        wait_for_release();
        return;
    }

    printf("This installs the QWERTY remap hook.\n\n");
    printf("Press Y to install, ESC to cancel.\n");
    wait_for_release();

    while (1) {
        wait_key_pressed();
        if (isKeyPressed(KEY_NSPIRE_ESC)) {
            wait_for_release();
            return;
        }
        if (isKeyPressed(KEY_NSPIRE_Y)) {
            break;
        }
        wait_for_release();
    }

    nl_set_resident();
    HOOK_INSTALL(SEND_TO_EVENT_QUEUE_CAS_CXII_620333, qwerty_event_queue_hook);
    _exit(0);
}

int main(void) {
    print_header();
    printf("1: Probe key events\n");
    printf("2: Install QWERTY remap hook\n");
    printf("ESC: Exit\n");

    switch (wait_menu_key()) {
        case 1:
            run_probe();
            break;
        case 2:
            install_hook();
            break;
        default:
            break;
    }

    return 0;
}
