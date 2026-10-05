#include <os.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern unsigned int nl_osid(void);

void _init(void) {}
void _fini(void) {}

#define OSID_CAS_CXII_620333 46u
#define OS_SIGNATURE_CAS_CXII_620333 0x1042AE10u
#define SEND_TO_EVENT_QUEUE_CAS_CXII_620333 0x1042D17Cu
#define SEND_TO_EVENT_QUEUE_WORD0_CAS_CXII_620333 0xE92D4070u
#define SEND_TO_EVENT_QUEUE_WORD1_CAS_CXII_620333 0xE59F4058u

/* ======================================================================
 *  EDIT HERE: the key remap table
 * ====================================================================== */

/*
 * A key is identified by BOTH its "key" value and its "ascii" value,
 * because several keys share a key value (Q/U, R/V, S/W all report
 * key 0x62/0x42/0x22) and only differ in ascii.
 *
 *   from_key, from_ascii : what the key reports now (viewer: key, ascii)
 *   to_key,   to_ascii   : what it should report instead
 *
 * Keep the 0xFFFFFFFF line LAST: it marks the end of the table.
 */
struct key_remap {
    uint32_t from_key;
    uint32_t from_ascii;
    uint32_t to_key;
    uint32_t to_ascii;
};

static const struct key_remap KEY_REMAP[] = {
    /* top row: Q W E R T Y U I O */
    { 0xA4, 0x00, 0x62, 0x71 },  /* EE   -> Q */
    { 0x66, 0x61, 0x22, 0x77 },  /* A    -> W */
    { 0x46, 0x62, 0x65, 0x65 },  /* B    -> E */
    { 0x26, 0x63, 0x42, 0x72 },  /* C    -> R */
    { 0x85, 0x64, 0x81, 0x74 },  /* D    -> T */
    { 0x65, 0x65, 0x60, 0x79 },  /* E    -> Y */
    { 0x45, 0x66, 0x62, 0x75 },  /* F    -> U */
    { 0x25, 0x67, 0x64, 0x69 },  /* G    -> I */
    { 0x79, 0x00, 0x23, 0x6F },  /* ?!>  -> O */

    /* middle row: A S D F G H J K L */
    { 0x72, 0x00, 0x66, 0x61 },  /* pi   -> A */
    { 0x84, 0x68, 0x22, 0x73 },  /* H    -> S */
    { 0x64, 0x69, 0x85, 0x64 },  /* I    -> D */
    { 0x44, 0x6A, 0x45, 0x66 },  /* J    -> F */
    { 0x24, 0x6B, 0x25, 0x67 },  /* K    -> G */
    { 0x83, 0x6C, 0x84, 0x68 },  /* L    -> H */
    { 0x63, 0x6D, 0x44, 0x6A },  /* M    -> J */
    { 0x43, 0x6E, 0x24, 0x6B },  /* N    -> K */
    { 0xA7, 0x00, 0x83, 0x6C },  /* flag -> L */

    /* bottom row: Z X C V B N M P */
    { 0xA0, 0x2C, 0x40, 0x7A },  /* ,    -> Z */
    { 0x23, 0x6F, 0x80, 0x78 },  /* O    -> X */
    { 0x82, 0x70, 0x26, 0x63 },  /* P    -> C */
    { 0x62, 0x71, 0x42, 0x76 },  /* Q    -> V */
    { 0x42, 0x72, 0x46, 0x62 },  /* R    -> B */
    { 0x22, 0x73, 0x43, 0x6E },  /* S    -> N */
    { 0x81, 0x74, 0x63, 0x6D },  /* T    -> M */
    { 0x62, 0x75, 0x82, 0x70 },  /* U    -> P */

    /* freed keys get the displaced specials */
    { 0x42, 0x76, 0xA4, 0x00 },  /* V    -> EE */
    { 0x22, 0x77, 0x72, 0x00 },  /* W    -> pi */
    { 0x80, 0x78, 0xA0, 0x2C },  /* X    -> , */
    { 0x60, 0x79, 0xA7, 0x00 },  /* Y    -> flag */
    { 0x40, 0x7A, 0x79, 0x00 },  /* Z    -> ?!> */

    { 0xFFFFFFFFu, 0, 0, 0 }
};

/* 1 = once KEY_REMAP has entries, opening the program installs the remap
 * immediately (no menu, no confirmation). 0 = always show the menu. */
#define AUTO_INSTALL 0

/* ====================================================================== */

static int pointer_is_plausible_event_buffer(const void *ptr) {
    uintptr_t addr = (uintptr_t)ptr;

    return (addr >= 0x10000000u && addr < 0x14000000u) ||
           (addr >= 0xA0000000u && addr < 0xB0000000u);
}

/* ---------------------------------------------------------------------
 *  Remap hook (no global data, no big static arrays)
 * ------------------------------------------------------------------- */

static const struct key_remap *lookup_remap(uint32_t key, uint32_t ascii) {
    const struct key_remap *r;

    for (r = KEY_REMAP; r->from_key != 0xFFFFFFFFu; r++) {
        if (r->from_key == key && r->from_ascii == ascii)
            return r;
    }
    return 0;
}

static void rewrite_key_event(struct s_ns_event *event) {
    const struct key_remap *remap;

    if (!event || !pointer_is_plausible_event_buffer(event))
        return;

    if (event->type != 0x8 && event->type != 0x10)
        return;

    if (event->modifiers & 0x4)
        return;

    remap = lookup_remap((uint32_t)event->key, (uint32_t)event->ascii);
    if (remap) {
        event->key = remap->to_key;
        event->ascii = (uint16_t)remap->to_ascii;
    }
}

HOOK_DEFINE(remap_event_queue_hook) {
    struct s_ns_event *event = (struct s_ns_event *)HOOK_SAVED_REGS(remap_event_queue_hook)[0];
    rewrite_key_event(event);
    HOOK_RESTORE_RETURN(remap_event_queue_hook);
}

/* ------------------------------------------------------------------- */

static void print_header(void) {
    printf("Key Remap\n\n");
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
        if (isKeyPressed(KEY_NSPIRE_3)) return 3;
        if (isKeyPressed(KEY_NSPIRE_ESC)) return 0;
        wait_for_release();
    }
}

static void refuse_unsupported(void) {
    printf("Refusing: OS/model/function\n");
    printf("fingerprint is not the verified\n");
    printf("CX II CAS OS 6.2.0.333 target.\n");
    printf("(If a hook is installed, use\n");
    printf("option 3 to remove it first.)\n\n");
}

/* Returns 1 if the user pressed Y, 0 if ESC. */
static int confirm_install(void) {
    printf("Press Y to install, ESC to cancel.\n");
    wait_for_release();

    while (1) {
        wait_key_pressed();
        if (isKeyPressed(KEY_NSPIRE_ESC)) {
            wait_for_release();
            return 0;
        }
        if (isKeyPressed(KEY_NSPIRE_Y))
            return 1;
        wait_for_release();
    }
}

static int table_is_empty(void) {
    return KEY_REMAP[0].from_key == 0xFFFFFFFFu;
}

static void install_hook(int ask) {
    printf("\nInstalling key remap hook...\n\n");

    if (!environment_is_supported()) {
        refuse_unsupported();
        return;
    }

    if (ask && !confirm_install())
        return;

    nl_set_resident();
    HOOK_INSTALL(SEND_TO_EVENT_QUEUE_CAS_CXII_620333, remap_event_queue_hook);
    printf("Hook installed.\n");
}

static void uninstall_hook(void) {
    volatile uint32_t *target = (volatile uint32_t *)SEND_TO_EVENT_QUEUE_CAS_CXII_620333;

    printf("\nRemoving key remap hook...\n\n");

    if (nl_osid() != OSID_CAS_CXII_620333 ||
        *(volatile uint32_t *)0x10000020u != OS_SIGNATURE_CAS_CXII_620333) {
        printf("Wrong OS: touching nothing.\n");
        return;
    }

    if (environment_is_supported()) {
        printf("No hook is installed.\n");
        return;
    }

    target[0] = SEND_TO_EVENT_QUEUE_WORD0_CAS_CXII_620333;
    target[1] = SEND_TO_EVENT_QUEUE_WORD1_CAS_CXII_620333;
    clear_cache();

    printf("Hook removed.\n");
}

/* Live key viewer: prints each key event. ESC (ascii 27) stops it. */
static void run_key_viewer(void) {
    struct s_ns_event ev;
    int done = 0;

    printf("\nLive key viewer. Press keys.\n");
    printf("ESC stops; then any key exits.\n");
    printf("type key ascii mods\n");
    wait_for_release();

    while (!done) {
        memset(&ev, 0, sizeof(ev));
        get_event(&ev);

        /* Skip the periodic idle event (20 12 12 0) and empty results. */
        if (ev.type == 0x20 && ev.key == 0x12 && ev.ascii == 0x12)
            continue;
        if (!(ev.type || ev.key || ev.ascii))
            continue;

        printf("%lX %lX %lX %lX\n",
               (unsigned long)ev.type,
               (unsigned long)ev.key,
               (unsigned long)ev.ascii,
               (unsigned long)ev.modifiers);

        if (ev.ascii == 27)
            done = 1;
    }

    msleep(600);        /* let any pending OS repaint finish first */
    wait_for_release();
}

static void pause_confirm(void) {
    int pressed = 0;
    printf("Press any key to continue.\n");

    while (!pressed) {
        for (int t = 0; t < 40; t++) {
            if (any_key_pressed()) {
                pressed = 1;
                break;
            }
            msleep(25);
        }
    }
    wait_for_release();
}

int main(void) {
    if (AUTO_INSTALL && !table_is_empty()) {
        install_hook(0);
        return 0;
    }

    print_header();
    printf("1: Install key remap hook\n");
    printf("2: Live key viewer\n");
    printf("3: Uninstall key remap hook\n");
    printf("ESC: Exit\n");

    switch (wait_menu_key()) {
        case 1:
            install_hook(1);
            pause_confirm();
            break;
        case 2:
            msleep(500);
            run_key_viewer();
            pause_confirm();
            break;
        case 3:
            uninstall_hook();
            pause_confirm();
            break;
        default:
            break;
    }

    return 0;
}