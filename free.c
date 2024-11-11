void pml_logic_loop(void) {
    // Base case: if the conversation is complete, exit the loop
    if (conversation_complete) {
        return;
    }
    // Recursive case: call the pml_logic_loop function again
    pml_logic_loop();
}
