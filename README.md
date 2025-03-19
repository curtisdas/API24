# Algorithms and Data Structures Project 2023-2024 (PoliMI) by Curtis Das

## Project Overview
This constitutes one of the three final projects for the Bachelor's in Engineering of Computing Systems at Politecnico di Milano.
The project is managed by **Prof. Alessandro Barenghi**.
The goal is to implement the software for managing a pastry shop (order management, stock management, recipe management, ...) using C, while optimizing time and memory efficciency.

## Tools Used
- Language: C11 (standard library, no multithreading)
- Editor: VSCode
- Testing: GDB, Valgrind, Callgrind,  Massif

## Specifications
The entire simulation takes place in discrete time steps. It is assumed that after each received command is executed, one time step passes. The simulation starts at time 0.
Each command is recieved as a stdio string which can be one of the following:
- 'aggiungi_ricetta ⟨nome_ricetta⟩ ⟨nome_ingrediente⟩ ⟨quantità⟩...' (add_ingredient ⟨recipe_name⟩ ⟨ingredient_name⟩ ⟨quantity⟩...)
Adds a recipe to the catalog. The number of ingredient-quantity pairs is arbitrary. If a recipe with the same name already exists, it is ignored.
Expected output: 'aggiunta' (added) or 'ignorato' (ignored).







