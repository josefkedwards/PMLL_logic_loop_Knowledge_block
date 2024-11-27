#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Global whitelist and blacklist
char *whitelist[] = {"62.0.0.1", "62.0.0.2"};
int whitelist_count = 2;
char *blacklist[MAX_BLACKLIST];
int blacklist_count = 0;

void watcher_init() {
    printf("[Watcher] Initializing...\n");
    system("sudo apt-get update");
    system("sudo apt-get install fail2ban -y");
    system("sudo apt-get install ufw -y");
    printf("[Watcher] Security tools installed and configured.\n");
}

void monitor_logs() {
    FILE *log_file = fopen("/var/log/auth.log", "r");
    if (!log_file) {
        perror("[Watcher] Failed to open log file");
        return;
    }
    fseek(log_file, 0, SEEK_END);  // Start at the end
    char buffer[1024];
    while (1) {
        if (fgets(buffer, sizeof(buffer), log_file)) {
            if (strstr(buffer, "Failed password")) {
                char ip[16];
                sscanf(buffer, "%*s %*s %*s %*s %s", ip);  // Extract IP
                handle_failed_attempt(ip);
            }
        }
        sleep(1);
    }
    fclose(log_file);
}

void handle_failed_attempt(const char *ip) {
    if (is_whitelisted(ip)) return;

    // Log the first attempt
    FILE *log = fopen("/var/log/failed_attempts.log", "a");
    if (log) {
        fprintf(log, "Failed login attempt from %s at %ld\n", ip, time(NULL));
        fclose(log);
    }

    // Message for the first attempt
    printf("[Watcher] State your case on why you accessed the back door. Your activity has been logged.\n");

    // Add additional functions to snoop data and record input
    snoop_data_streams(ip);
    record_user_input(ip);

    // Handle second attempt and block if necessary
    add_to_blacklist(ip);
    printf("[Watcher] Warning! IP %s flagged.\n", ip);
}

void snoop_data_streams(const char *ip) {
    printf("[Watcher] Snoop data streams for IP: %s\n", ip);
    char command[256];
    snprintf(command, sizeof(command), "sudo tcpdump -i eth0 host %s -w capture.pcap", ip);
    system(command);  // Capture traffic from the specific IP
}

void record_user_input(const char *ip) {
    printf("[Watcher] Record user input for IP: %s\n", ip);
}

int is_whitelisted(const char *ip) {
    for (int i = 0; i < whitelist_count; i++) {
        if (strcmp(ip, whitelist[i]) == 0) return 1;
    }
    return 0;
}

void add_to_blacklist(const char *ip) {
    if (blacklist_count < MAX_BLACKLIST) {
        blacklist[blacklist_count++] = strdup(ip);
    }
    printf("[Watcher] IP %s added to blacklist.\n", ip);

    // Add IP to the firewall blacklist using ufw
    char command[256];
    snprintf(command, sizeof(command), "sudo ufw deny from %s", ip);
    system(command);

    // Add IP to fail2ban blacklist
    snprintf(command, sizeof(command), "sudo fail2ban-client set sshd banip %s", ip);
    system(command);

    // Save the blacklist to disk for persistence
    save_blacklist();
}

void save_blacklist() {
    FILE *file = fopen("/var/log/blacklist.txt", "w");
    for (int i = 0; i < blacklist_count; i++) {
        fprintf(file, "%s\n", blacklist[i]);
    }
    fclose(file);
}
