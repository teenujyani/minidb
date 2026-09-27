# MiniDB

A lightweight relational database built from scratch in C++17 to understand how database systems work internally.

> 🚧 MiniDB is currently under active development.

The goal of this project is to build a functional database system from the ground up instead of relying on an existing database engine. The project focuses on understanding the internal mechanisms behind storage, indexing, buffering, query processing, and database execution.

---

## 🎯 Project Goals

MiniDB is being developed as a learning-focused database system with the following goals:

- Understand how databases store data on disk
- Implement page-based storage
- Build a disk manager from scratch
- Implement serialization and deserialization
- Build a buffer pool and page cache
- Implement LRU page replacement
- Build a B+ Tree index from scratch
- Implement table and record storage
- Build a SQL lexer and parser
- Implement basic SQL query execution
- Understand transactions and concurrency
- Explore database recovery and logging
- Provide a simple command-line interface

The project is designed to gradually move from low-level storage mechanisms to a complete database execution pipeline.

---

## 🏗️ Planned Architecture

```text
                         ┌───────────────────┐
                         │      SQL CLI       │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │       Lexer       │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │      Parser       │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │  Query Executor   │
                         └─────────┬─────────┘
                                   │
                    ┌──────────────┴──────────────┐
                    │                             │
                    ▼                             ▼
             ┌──────────────┐              ┌──────────────┐
             │ Table Storage│              │   B+ Tree    │
             └──────┬───────┘              └──────┬───────┘
                    │                             │
                    └──────────────┬──────────────┘
                                   ▼
                         ┌───────────────────┐
                         │    Buffer Pool    │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │   Disk Manager    │
                         └─────────┬─────────┘
                                   │
                                   ▼
                         ┌───────────────────┐
                         │   Database File   │
                         └───────────────────┘