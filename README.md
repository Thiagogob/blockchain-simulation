# Simplified C Blockchain

---

## 📜 About

This project is a simulation of a simplified blockchain, implemented in the C programming language. The program mimics the process of **block mining**, **transaction recording**, and **address balance management**. The primary goal is to demonstrate how a blockchain functions at a fundamental level, using data structures and algorithms to ensure data integrity and persistence.

The application includes a Proof of Work-based mining system where miners compete to find a nonce that results in a hash with a specific characteristic (in this case, two leading hexadecimal zeros). Mined blocks are stored in binary files, and indexes are created to allow for efficient data searching.

The program uses the OpenSSL library for the **SHA256 hashing function** and a library for generating pseudo-random numbers. An interactive menu lets users perform various queries and analyses on the simulated blockchain data.

---

## 💻 How It Works

The project operates in a continuous cycle of block creation and mining. The main process is as follows:

1.  **Block Generation**: The program generates new blocks, which include fictitious transactions between addresses. The first block, the "genesis block," is pre-defined.
2.  **Mining**: For each new block, a random "miner" address is selected. The mining function repeatedly calculates the block's hash, changing the nonce value until the hash meets the difficulty criterion (starting with two leading hexadecimal zeros).
3.  **Validation and Reward**: When a valid hash is found, the block is considered mined. The miner receives a reward of 50 BTC. The balances of the addresses involved in the block's transactions are updated.
4.  **Data Persistence**: The mined blocks are saved to a binary file (`blocos_minerados.bin`). To optimize searches, two additional index files are created:
    * `arquivo_indices_minerador2.bin`: For searching blocks by miner's address.
    * `arquivo_indices_nonce2.bin`: For searching blocks by nonce.
5.  **Interactive Interface**: A terminal menu offers the user various options to interact with the blockchain, such as checking addresses with the largest balances, finding blocks with the most transactions, and searching for specific blocks.

---

## 📈 Data Structures and Algorithms

This project uses several data structures and algorithms to efficiently manage and manipulate blockchain data:

### Data Structures:

* **`struct` and `typedef`**: The project is built on defining structs to represent blocks (`BlocoNaoMinerado`, `BlocoMinerado`), index nodes (`TIndiceMinerador2`, `TListaIndiceNonce2`), and the list of addresses with a balance (`listaBTC`).
* **Linked List (`listaBTC`)**: Used to maintain a dynamic list of all addresses that have a positive BTC balance. This optimizes the transaction creation process by only selecting valid sender addresses.
* **Binary Search Tree (BST) (`TIndiceMinerador2`)**: A **binary search tree** is built in memory to index blocks based on the miner's address. This allows for much faster lookups of blocks mined by a specific address compared to a linear search through the entire file.
* **Hash Table (`TListaIndiceNonce2`)**: A **hash table** is used to index blocks by their nonce. When the index is loaded into memory, each nonce is mapped to a "bucket" in the table, where a linked list stores the index nodes. This enables extremely fast searches for all blocks with a specific nonce.

### Algorithms:

* **Hashing (SHA256)**: Used for the blockchain's security and integrity. Each block's hash depends on the previous block's hash, creating an immutable chain of blocks. Mining is essentially the search for a valid hash that meets a difficulty criterion.
* **Merge Sort**: A **divide-and-conquer sorting algorithm** used to sort blocks based on the number of transactions they contain. This is useful for queries that require viewing blocks with the most or fewest transactions.
* **Binary Search**: Employed to efficiently search for a block by its number within the 16-block "chunks" loaded into memory. This approach is significantly faster than a sequential search.

The combination of these data structures and algorithms ensures that, even with a large number of blocks, data queries and manipulations remain efficient.
