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
 *  KEY DICTIONARY: the keys the on-calculator editor knows about.
 *  key/ascii are what the live viewer reports for that key.
 *  To support more keys: add a row here, add a matching test in
 *  scan_named_key(), and raise NAMED_COUNT.
 * ====================================================================== */

struct named_key {
    char name[6];
    uint32_t key;
    uint32_t ascii;
};

#define NAMED_COUNT 30

static const struct named_key NAMED[NAMED_COUNT] = {
    { "A", 0x66, 0x61 }, { "B", 0x46, 0x62 }, { "C", 0x26, 0x63 },
    { "D", 0x85, 0x64 }, { "E", 0x65, 0x65 }, { "F", 0x45, 0x66 },
    { "G", 0x25, 0x67 }, { "H", 0x84, 0x68 }, { "I", 0x64, 0x69 },
    { "J", 0x44, 0x6A }, { "K", 0x24, 0x6B }, { "L", 0x83, 0x6C },
    { "M", 0x63, 0x6D }, { "N", 0x43, 0x6E }, { "O", 0x23, 0x6F },
    { "P", 0x82, 0x70 }, { "Q", 0x62, 0x71 }, { "R", 0x42, 0x72 },
    { "S", 0x22, 0x73 }, { "T", 0x81, 0x74 }, { "U", 0x62, 0x75 },
    { "V", 0x42, 0x76 }, { "W", 0x22, 0x77 }, { "X", 0x80, 0x78 },
    { "Y", 0x60, 0x79 }, { "Z", 0x40, 0x7A },
    { "EE",   0xA4, 0x00 },   /* 26 */
    { "PI",   0x72, 0x00 },   /* 27 */
    { "COMMA",0xA0, 0x2C },   /* 28 */
    { "FLAG", 0xA7, 0x00 }    /* 29 */
};

/* Which physical key is being pressed right now? Returns an index into
 * NAMED, or -1. If the compiler rejects a KEY_NSPIRE_ name on one of the
 * last four lines, just delete that line. */
static int scan_named_key(void) {
    if (isKeyPressed(KEY_NSPIRE_A)) return 0;
    if (isKeyPressed(KEY_NSPIRE_B)) return 1;
    if (isKeyPressed(KEY_NSPIRE_C)) return 2;
    if (isKeyPressed(KEY_NSPIRE_D)) return 3;
    if (isKeyPressed(KEY_NSPIRE_E)) return 4;
    if (isKeyPressed(KEY_NSPIRE_F)) return 5;
    if (isKeyPressed(KEY_NSPIRE_G)) return 6;
    if (isKeyPressed(KEY_NSPIRE_H)) return 7;
    if (isKeyPressed(KEY_NSPIRE_I)) return 8;
    if (isKeyPressed(KEY_NSPIRE_J)) return 9;
    if (isKeyPressed(KEY_NSPIRE_K)) return 10;
    if (isKeyPressed(KEY_NSPIRE_L)) return 11;
    if (isKeyPressed(KEY_NSPIRE_M)) return 12;
    if (isKeyPressed(KEY_NSPIRE_N)) return 13;
    if (isKeyPressed(KEY_NSPIRE_O)) return 14;
    if (isKeyPressed(KEY_NSPIRE_P)) return 15;
    if (isKeyPressed(KEY_NSPIRE_Q)) return 16;
    if (isKeyPressed(KEY_NSPIRE_R)) return 17;
    if (isKeyPressed(KEY_NSPIRE_S)) return 18;
    if (isKeyPressed(KEY_NSPIRE_T)) return 19;
    if (isKeyPressed(KEY_NSPIRE_U)) return 20;
    if (isKeyPressed(KEY_NSPIRE_V)) return 21;
    if (isKeyPressed(KEY_NSPIRE_W)) return 22;
    if (isKeyPressed(KEY_NSPIRE_X)) return 23;
    if (isKeyPressed(KEY_NSPIRE_Y)) return 24;
    if (isKeyPressed(KEY_NSPIRE_Z)) return 25;
    if (isKeyPressed(KEY_NSPIRE_EE)) return 26;
    if (isKeyPressed(KEY_NSPIRE_PI)) return 27;
    if (isKeyPressed(KEY_NSPIRE_COMMA)) return 28;
    if (isKeyPressed(KEY_NSPIRE_FLAG)) return 29;
    return -1;
}

static int named_index(uint32_t key, uint32_t ascii) {
    int i;
    for (i = 0; i < NAMED_COUNT; i++) {
        if (NAMED[i].key == key && NAMED[i].ascii == ascii)
            return i;
    }
    return -1;
}

/* ======================================================================
 *  KEY MAP
 *
 *  DEFAULT_MAP is what you get after "reset to defaults" and at startup.
 *  g_map is the live copy: the on-calculator editor changes it, and the
 *  hook reads it. Each row: from_key, from_ascii, to_key, to_ascii.
 *  (Q/U, R/V, S/W share a key value, so ascii is part of the match.)
 * ====================================================================== */

struct key_remap {
    uint32_t from_key;
    uint32_t from_ascii;
    uint32_t to_key;
    uint32_t to_ascii;
};

#define END_MARK 0xFFFFFFFFu
#define MAP_MAX 64

static const struct key_remap DEFAULT_MAP[] = {
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

    { END_MARK, 0, 0, 0 }
};

/* Live table. It has a non-zero initialiser on purpose, so it is stored
 * as initialised data and not as .bss. */
static struct key_remap g_map[MAP_MAX] = { { END_MARK, 0, 0, 0 } };

/* 1 = installs the current map immediately when the program opens
 * (no menu). 0 = always show the menu. */
#define AUTO_INSTALL 0

/* ====================================================================== */

static int pointer_is_plausible_event_buffer(const void *ptr) {
    uintptr_t addr = (uintptr_t)ptr;

    return (addr >= 0x10000000u && addr < 0x14000000u) ||
           (addr >= 0xA0000000u && addr < 0xB0000000u);
}

/* ---------------------------------------------------------------------
 *  Remap hook (reads g_map; never writes anything)
 * ------------------------------------------------------------------- */

static const struct key_remap *lookup_remap(uint32_t key, uint32_t ascii) {
    unsigned int i;

    for (i = 0; i < MAP_MAX; i++) {
        const struct key_remap *r = &g_map[i];

        if (r->from_key == END_MARK)
            break;
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

/* ---------------------------------------------------------------------
 *  Map editing helpers (run in the normal program, not in the hook)
 * ------------------------------------------------------------------- */

static unsigned int map_count(void) {
    unsigned int n = 0;
    while (n < MAP_MAX && g_map[n].from_key != END_MARK)
        n++;
    return n;
}

static void map_clear(void) {
    g_map[0].from_key = END_MARK;
}

static void map_load_defaults(void) {
    unsigned int n = 0;

    while (DEFAULT_MAP[n].from_key != END_MARK && n < MAP_MAX - 1) {
        g_map[n] = DEFAULT_MAP[n];
        n++;
    }
    g_map[n].from_key = END_MARK;
}

/* Make key src act as key dst. src == dst removes src's row.
 * Returns a short message. */
static const char *map_set(int src, int dst) {
    unsigned int n = map_count();
    unsigned int i;

    for (i = 0; i < n; i++) {
        if (g_map[i].from_key == NAMED[src].key &&
            g_map[i].from_ascii == NAMED[src].ascii)
            break;
    }

    if (src == dst) {
        if (i == n)
            return "nothing to remove";
        for (; i + 1 < n; i++)
            g_map[i] = g_map[i + 1];
        g_map[n - 1].from_key = END_MARK;
        return "row removed";
    }

    if (i == n) {
        if (n >= MAP_MAX - 1)
            return "map is full";
        g_map[n + 1].from_key = END_MARK;
        n++;
    }

    g_map[i].from_key = NAMED[src].key;
    g_map[i].from_ascii = NAMED[src].ascii;
    g_map[i].to_key = NAMED[dst].key;
    g_map[i].to_ascii = NAMED[dst].ascii;
    return "saved";
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
        if (isKeyPressed(KEY_NSPIRE_4)) return 4;
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

static void pause_confirm(void) {
    printf("Press any key to continue.\n");
    wait_for_release();
    wait_key_pressed();
    wait_for_release();
}

static void install_hook(int ask) {
    printf("\nInstalling key remap hook...\n");
    printf("(%u rows in the map)\n\n", map_count());

    if (!environment_is_supported()) {
        refuse_unsupported();
	pause_confirm();
        return;
    }

    if (ask && !confirm_install())
        return;

    HOOK_INSTALL(SEND_TO_EVENT_QUEUE_CAS_CXII_620333,
                 remap_event_queue_hook);

    printf("Hook installed.\n");

    if (ask)
        pause_confirm();

    nl_set_resident();
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
    printf("Screen will return to file manager and flicker when \npressing keys. This is normal.\n\n");
    wait_for_release();
    pause_confirm();

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

/* ---------------------------------------------------------------------
 *  On-calculator editor. It only uses wait_key_pressed()/isKeyPressed()
 *  (like the menu), never get_event(), so the screen stays stable.
 * ------------------------------------------------------------------- */

#define KEY_ESC_CODE   (-2)
#define KEY_LIST_CODE  (-3)
#define KEY_DEFS_CODE  (-4)
#define KEY_CLEAR_CODE (-5)

/* Returns an index into NAMED, -1 for an unknown key, or one of the
 * KEY_*_CODE values for ESC and the command keys 1, 2, 3. */
static int wait_named_key(void) {
    int idx;

    wait_key_pressed();
    if (isKeyPressed(KEY_NSPIRE_ESC))
        idx = KEY_ESC_CODE;
    else if (isKeyPressed(KEY_NSPIRE_1))
        idx = KEY_LIST_CODE;
    else if (isKeyPressed(KEY_NSPIRE_2))
        idx = KEY_DEFS_CODE;
    else if (isKeyPressed(KEY_NSPIRE_3))
        idx = KEY_CLEAR_CODE;
    else
        idx = scan_named_key();

    wait_for_release();
    return idx;
}

static void print_key_name(uint32_t key, uint32_t ascii, char *buf) {
    int i = named_index(key, ascii);

    if (i >= 0)
        sprintf(buf, "%s", NAMED[i].name);
    else
        sprintf(buf, "%lX/%lX", (unsigned long)key, (unsigned long)ascii);
}

static void list_map(void) {
    unsigned int n = map_count();
    unsigned int i;
    char a[16];
    char b[16];

    printf("\n%u rows:\n", n);
    for (i = 0; i < n; i++) {
        print_key_name(g_map[i].from_key, g_map[i].from_ascii, a);
        print_key_name(g_map[i].to_key, g_map[i].to_ascii, b);
        printf("%s -> %s\n", a, b);

        if ((i + 1) % 11 == 0 && i + 1 < n) {
            printf("-- any key: more --\n");
            wait_key_pressed();
            wait_for_release();
        }
    }
    printf("(end of list)\n");
}

static void edit_map(void) {
    int src, dst;

    printf("\nEdit key map.\n");
    printf("Press the key to CHANGE, then\n");
    printf("the key it should ACT AS.\n");
    printf("Same key twice = remove its row.\n");
    printf("1 list  2 defaults  3 clear all\n");
    printf("ESC = done\n\n");
    wait_for_release();

    while (1) {
        printf("Key to change?\n");
        src = wait_named_key();

        if (src == KEY_ESC_CODE)
            break;
        if (src == KEY_LIST_CODE) {
            list_map();
            continue;
        }
        if (src == KEY_DEFS_CODE) {
            map_load_defaults();
            printf("Defaults restored.\n");
            continue;
        }
        if (src == KEY_CLEAR_CODE) {
            map_clear();
            printf("Map cleared.\n");
            continue;
        }
        if (src < 0) {
            printf("Unknown key (not in dictionary).\n");
            continue;
        }

        printf("%s acts as?\n", NAMED[src].name);
        dst = wait_named_key();

        if (dst == KEY_ESC_CODE) {
            printf("Cancelled.\n");
            continue;
        }
        if (dst < 0) {
            printf("Unknown key, cancelled.\n");
            continue;
        }

        printf("%s -> %s: %s\n", NAMED[src].name, NAMED[dst].name, map_set(src, dst));
    }
}

int main(void) {
    map_load_defaults();

    if (AUTO_INSTALL && map_count() > 0) {
        install_hook(0);
        return 0;
    }

    print_header();
    printf("1: Install key remap hook\n");
    printf("2: Edit key map\n");
    printf("3: Uninstall key remap hook\n");
    printf("4: Live key viewer (Diagnostic)\n");
    printf("ESC: Exit\n");

    switch (wait_menu_key()) {
        case 1:
            install_hook(1);
            break;
        case 2:
            edit_map();
            printf("\n1: Install now   other: exit\n");
            wait_key_pressed();
            if (isKeyPressed(KEY_NSPIRE_1)) {
                wait_for_release();
                install_hook(1);
                pause_confirm();
            }
            break;
        case 3:
            uninstall_hook();
            pause_confirm();
            break;
        case 4:
            msleep(500);
            run_key_viewer();
            pause_confirm();
            break;
        default:
            break;
    }

    return 0;
}