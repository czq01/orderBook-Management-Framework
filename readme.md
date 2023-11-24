# OrderBook System

Name: OrderBook System
Author: czq01

project dependency: `g++>=9`, `Cmake>=3.9.0`

## Structure

We have three part of component:
- Main Engine
- Book Processor
- Disk Data Manager

### Main Engine

`Main Engine` responsible for interprete queries and execution, as well as new data loading.

### Book Processor

`Book Processor` is threaded, provides apis for new data processing and query data fetching.

### Disk Data Manager

`Disk Data Manager` is responsible for Data storage on the disk.

## Usage

use load API on the main engine to load and preprocess new order data.

use query API on the main engine to fetch snapshot.