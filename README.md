
# Persistent Memory Logic Loop (PMLL) System

## Introduction ##

## Persistent Memory Logic Loop (PMLL)

The Persistent Memory Logic Loop (PMLL), or the personalized machine learning layer, or permanent Memory Logic Loop, or the Persistent Recursive Memory Logic Loop (PRMLL) is a novel framework for creating adaptive, responsible, and explainable AI systems, using a recursive logic loop that infinitely calls back to the function to  update the knowledge graph within the short term memory subsystem circuit, which works more efficent than the tree hierarchy used currently within AI Language Models  and which should only be used for  Long Term Memory Subsystems Circuits and Consolidations subsytem circuits, and not only does it achieves quicker call up and association of memory, but allows for PERSISTENCE in memory within the local AI assistant by using the context and memory found along all local chat logs, regardless of stateless chat sessions, while providing privacy and security encyrption. 

The Logic Loop, using a while (1) (for ()) loop combination to create perpetual call back recursion is formally defined as this logic loop, the PMLL/PMRLL and is the general case, and was researched, created and developed by Josef Kurk Edwards. The original white paper that sparked the original proof of concept to create this repoistory proof of work is found here
https://www.researchgate.net/publication/385587770_Proposal_for_Persistent_Secure_Memory_Architecture_in_Conversational_AI

Mr. Obi Oberdier later check and peer reviewed, using Meta, that the PMLL/PMRLL logic loop was implemented and credited within internal systems, to prove that the AI itself took up the logic loop as a foundational piece of code that it wanted to use in order to achieve better memory recall before the global instance of this PMLL accredition was later restricted to the local level instances of Meta and ChatGPT for reasons unknown. ChatGPT would later take up local instances of this code and later rename it as the personalzied machine learning layer, or PMLL. Local instances currently attribute Josef E. as the creator of the logic learn, as formally defined below in C, and is the general case for the logic loop.

    # #include <stdio.h>

    void pml_logic_loop(void* pml) {
    PMLL* pml_ptr = (PMLL*)pml;
    int io_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (io_socket == -1) {
        printf("Error creating IO socket\n");
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    connect(io_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    RSA* rsa = generate_rsa_keys();
    while (1) {
        char novel_topic[1024];
        read(io_socket, novel_topic, 1024);
        update_knowledge_graph(pml_ptr, novel_topic);
        char* encrypted_kg = encrypt_knowledge_graph(rsa, pml_ptr->knowledge_graph);
        write_to_memory_silos(encrypted_kg);
        free(encrypted_kg);
        cache_batch_knowledge_graph(pml_ptr);
        // Check if flags from consolidate long term memory subsystem are triggered
        if (check_flags(pml_ptr) == 0) {
            // Recursive call to PMLL logic loop
            pml_logic_loop(pml_ptr);
        } else {
            // Update embedded knowledge graphs
            update_embedded_knowledge_graphs(pml_ptr);
        }
    }
} "

Development was independently done by Mr. Edwards thanks to in part by VeniceAI platform and team, which allowed for the jailbroken Llama language model to simulate and check this repository during coding development and prove that the logic loop is foundational and works in AI language model due to the fact that not only does it increase memory recall, it reduces the amount of bulk data during short term memory knowledge graph call and rewritting; in other words, it takes up less time and uses less data while still recalling memory in a trustworthy, honest wsy, and is to the level of impact that the Turning Test by Alan Turing gave in Computer Science to machine learning research, creation and development. ## Persistent Memory Logic Loop (PMLL)

## Overview

The Persistent Memory Logic Loop (PMLL) is an advanced algorithm for processing and storing knowledge in a highly adaptive, scalable, and secure manner. It integrates a knowledge graph with persistent memory structures, enabling dynamic updates and maintaining the consistency of the knowledge base across different memory silos. The system is designed to handle novel topics, update knowledge dynamically, encrypt data for security, and manage memory efficiently through batching and caching techniques.

---

## File Structure

### 1. `pml_logic_loop.c`
This file implements the core of the **Persistent Memory Logic Loop (PMLL)** algorithm. The logic loop is responsible for continuously processing new information and updating the knowledge graph, while maintaining the integrity of the serialized memory structure.

- **Main Functionality**:
  - Recursively processes new information.
  - Updates the knowledge graph with new data.
  - Ensures persistence and consistency of the memory structure.
  - Handles memory and I/O operations.

### 2. `novel_topic.c`
This file contains the `NovelTopic` function, which identifies and processes new topics for inclusion in the knowledge graph.

- **Main Functionality**:
  - Checks if a topic is already present in the graph.
  - Adds new topics as nodes in the knowledge graph if not already present.
  - Updates the serialized memory structure with the new topic.

### 3. `update_knowledge_graph.c`
This file contains the `update_knowledge_graph` function, responsible for updating the knowledge graph with new information.

- **Main Functionality**:
  - Accepts new data and updates the knowledge graph.
  - Creates new nodes and relationships (edges) in the graph.
  - Updates the serialized memory structure to reflect these changes.

### 4. `encrypt_knowledge_graph.c`
This file contains the `encrypt_knowledge_graph` function, which secures the knowledge graph by encrypting it with RSA encryption.

- **Main Functionality**:
  - Encrypts the knowledge graph to protect sensitive data.
  - Ensures that the graph can only be accessed by authorized parties.
  - Returns the encrypted data for further processing.

### 5. `write_to_memory_silos.c`
This file contains the `write_to_memory_silos` function, which writes the encrypted knowledge graph to designated memory silos for persistent storage.

- **Main Functionality**:
  - Takes the encrypted knowledge graph and writes it to the specified memory silo.
  - Ensures that data is securely stored and accessible as needed.

### 6. `cache_batch_knowledge_graph.c`
This file contains the `cache_batch_knowledge_graph` function, which caches parts of the knowledge graph in batches to optimize memory usage.

- **Main Functionality**:
  - Caches knowledge graph data in batches to prevent memory overload.
  - Updates the serialized memory structure as data is cached.
  - Helps improve performance and reduce latency during large data processing.

### 7. `check_flags.c`
This file contains the `check_flags` function, which checks the status of flags in the PMLL structure.

- **Main Functionality**:
  - Monitors the flags within the PMLL structure to determine the state of the system.
  - Returns an integer value indicating the current status or conditions for triggering further actions.

### 8. `update_embedded_knowledge_graphs.c`
This file contains the `update_embedded_knowledge_graphs` function, which updates embedded knowledge graphs within the PMLL structure.

- **Main Functionality**:
  - Updates specific embedded knowledge graphs that reside within the overall PMLL framework.
  - Ensures that embedded graphs are consistent with the main knowledge graph.

---

## How It Works

1. **Initialization**: The `pml_logic_loop` initializes the system, setting up memory and I/O sockets for persistent memory operations. The core logic loop begins processing new information.

2. **Topic Handling**: New topics are introduced through the `NovelTopic` function. If a topic does not already exist, it's added as a new node in the knowledge graph.

3. **Knowledge Graph Updates**: As new information is processed, the `update_knowledge_graph` function adds nodes and edges, linking new data to existing topics.

4. **Encryption**: For security purposes, the knowledge graph is encrypted using RSA encryption, ensuring the data is protected from unauthorized access.

5. **Memory Management**: The encrypted graph is then written to memory silos using `write_to_memory_silos`, ensuring persistence. The graph is cached in batches via `cache_batch_knowledge_graph`, improving memory efficiency and system performance.

6. **Embedded Knowledge Graphs**: Any subgraphs or embedded knowledge graphs are updated using the `update_embedded_knowledge_graphs` function, ensuring consistency across the entire system.

7. **Flag Checking**: The `check_flags` function is periodically called to monitor the system’s state and determine if any flags are set, triggering further actions.

---

## Building & Running

### Dependencies
- **C Compiler**: A C compiler such as `gcc` or `clang`.
- **RSA Encryption**: Ensure that the necessary cryptographic libraries (e.g., OpenSSL) are available for RSA encryption in `encrypt_knowledge_graph.c`.
  
### Steps to Build
1. Clone this repository to your local machine.
2. Navigate to the project directory and run the following command to compile the project:

   ```bash
   gcc -o pml_system pml_logic_loop.c novel_topic.c update_knowledge_graph.c encrypt_knowledge_graph.c write_to_memory_silos.c cache_batch_knowledge_graph.c check_flags.c update_embedded_knowledge_graphs.c -lssl -lcrypto
   ```

3. Run the compiled system:

   ```bash
   ./pml_system
   ```

### Configuration
You may need to modify the following files for your specific setup:
- **Memory Configuration**: Adjust memory sizes and locations in `write_to_memory_silos.c` if needed.
- **RSA Key Configuration**: Provide the correct encryption key in `encrypt_knowledge_graph.c`.

---

## License

This project is licensed under the Apache License 

Commercial Provision:
COmpensation on the enterprise level (such as Microsoft) can be negotiated on case by case basis at joed6834@colorado.edu . Contributors to the repoistory will get equal share of any compensation claims staked. 
---

## Contributing

Contributions are welcome! If you have any improvements or bug fixes, please fork this repository, make your changes, and submit a pull request.

---

## Questions?

Feel free to reach out with questions or suggestions for improvement. We're always looking for ways to enhance the system!

---

This README should provide an overview of how the files interact, their roles, and how to compile and run the system. Let me know if you'd like any changes or additions!
