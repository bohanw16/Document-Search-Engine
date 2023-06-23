# Document Search Engine

**A Multi-Threaded Online Search Engine With A Query Processer For Local And Online Documents**

## Introduction

A search engine hosted on web server that allows users to search for local text files and online documents with key words.

## Getting Started

### Prerequisites
-   make
-   C++
-   Git

### Installation
-   Clone the repo
```bash
git clone https://github.com/bohanw16/DocumentSearchEngine.git
cd DocumentSearchEngine
```
-   Compile the software
```bash
cd main
make
```

### Try it out
-   Launch the web server with documents from directory $\texttt{../projdocs}$
```bash
./http333d 5555 ../projdocs unit_test_indices/*
```
-   Explore the search engine at port:5555
    -   Example when using a local machine:  http://localhost:5555/
    -   Example when using a virtual machine:  http://hostname.domain:5555/

### Recompile and clean up
-   If you want to recompile or clean up the compiled files, from $\texttt{/document-search-engine}$ run
```bash
make
make clean
```
