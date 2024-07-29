#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Function prototypes
void kernel_main();
void clear_screen();
void print(const char* str);
void handle_command(const char* command);
char get_char();
int strncmp(const char* s1, const char* s2, size_t n);
void* memset(void* s, int c, size_t n);

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

uint16_t* terminal_buffer;
uint16_t terminal_row;
uint16_t terminal_column;

void kernel_main() {
    clear_screen();
    print("Hello world from AjitOS Kernel!\nAjitOS> ");
    
    char command[128];
    size_t command_index = 0;

    while (true) {
        char c = get_char();
        if (c == '\n') {
            command[command_index] = '\0';
            print("\n");
            handle_command(command);
            print("AjitOS> ");
            command_index = 0;
        } else if (c == '\b') {
            if (command_index > 0) {
                command_index--;
                terminal_column--;
                terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] = (terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] & 0xFF00) | ' ';
            }
        } else {
            command[command_index++] = c;
            terminal_buffer[terminal_row * VGA_WIDTH + terminal_column++] = (0x07 << 8) | c;
        }
    }
}

void clear_screen() {
    terminal_buffer = (uint16_t*) VGA_ADDRESS;
    terminal_row = 0;
    terminal_column = 0;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = (0x07 << 8) | ' ';
        }
    }
}

void print(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            terminal_column = 0;
            terminal_row++;
        } else {
            terminal_buffer[terminal_row * VGA_WIDTH + terminal_column++] = (0x07 << 8) | str[i];
        }
    }
}

void handle_command(const char* command) {
    if (strncmp(command, "clear", 5) == 0) {
        clear_screen();
    } else if (strncmp(command, "echo ", 5) == 0) {
        print(command + 5);
        print("\n");
    } else if (strncmp(command, "ls", 2) == 0) {
        print("file1.txt\nfile2.txt\n");
    } else if (strncmp(command, "help", 4) == 0) {
        print("Available commands: echo, clear, ls, help\n");
    } else {
        print("Unknown command\n");
    }
}

char get_char() {
    // Simple implementation to get a character from the keyboard.
    // You will need to implement this function using the appropriate I/O ports.
    // For now, it is left as a placeholder.
    return 0;
}

// Custom implementation of strncmp
int strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        if (s1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

// Custom implementation of memset
void* memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}
