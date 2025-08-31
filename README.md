# Laboratory Work: Working with Structured Files with Indexed-Sequential Access (Without DBMS)

## Objective
Learn to operate with structured files **without a DBMS**, using only **C/C++**.

## Domain
**Film Industry** ER-model with entities:
- Movie
- Director
- Actor
- Genre

Chosen **1–∞** relationship:
- **Director → Movie** (a director can make many movies)

Thus:
- **Director** = master-file  
- **Movie** = slave-file  

---

## Program Functionality (CLI)

The application provides a command-line interface with the following commands:

### General
- `help` — Show available commands  
- `exit` — Exit program  

### Insert
- `insert-m <ID> <Name> <Nat>` — Insert a new director  
- `insert-s <ID> <MasterID> <Title>` — Insert a new movie for a director  

### Delete
- `del-m <ID>` — Delete a director and all related movies  
- `del-s <ID>` — Delete a specific movie  

### Update
- `update-m <ID> <Name> <Nat>` — Update director details  
- `update-s <ID> <Title>` — Update movie title  

### Read
- `get-m <ID>` — Get director by ID  
- `get-s <ID>` — Get movie by ID  

### Count
- `calc-m` — Count all directors and their movies  
- `calc-s` — Count all movies  

### Utilities
- `ut-m` — Print all directors (including metadata)  
- `ut-s` — Print all movies (including metadata)  

---

## Testing Scenario

A quick validation flow:

1. Insert **5 directors**  
2. For **3 directors**, insert 1, 2, and 3 movies respectively  
3. Run `ut-m`, `ut-s`  
4. Delete a director with 2 movies  
5. Delete one movie from a director who has 3 movies  
6. Run `ut-m`, `ut-s`  
7. Insert another director and one movie for them  
8. Run `ut-m`, `ut-s`  
9. Update a field in a director record and in a movie record  
10. Run `ut-m`, `ut-s`  

---

## Conclusion

This project demonstrates a simple data management system for the **Film Industry** domain using **structured files with indexed-sequential access** instead of a DBMS.  

The program provides a clear CLI for performing CRUD operations, listing, counting, and utility functions.
