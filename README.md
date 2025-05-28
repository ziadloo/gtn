<div align="center">
<img src="gtn.svg" alt="logo" width="300"></img>
</div>

# GTN: Automatic Differentiation with WFSTs

[**Quickstart**](#quickstart)
| [**Installation**](#installation)
| [**Documentation**](https://gtn.readthedocs.io/en/latest/)

[![facebookresearch](https://circleci.com/gh/facebookresearch/gtn.svg?style=shield&circle-token=5bc7bf18680cc891afda8dbabfdec02605131e17)](https://circleci.com/gh/facebookresearch/gtn)
[![Documentation Status](https://img.shields.io/readthedocs/gtn.svg)](https://gtn.readthedocs.io/en/latest/)

## What is GTN?

GTN is a framework for automatic differentiation with weighted finite-state
transducers. The framework is written in C++ and has bindings to
Python.

The goal of GTN is to make adding and experimenting with structure in learning
algorithms much simpler. This structure is encoded as weighted automata, either
acceptors (WFSAs) or transducers (WFSTs). With `gtn` you can dynamically construct complex
graphs from operations on simpler graphs. Automatic differentiation gives gradients with respect to any input or intermediate graph
with a single call to `gtn.backward`.

Also checkout the repository [gtn_applications](https://github.com/facebookresearch/gtn_applications) which consists of GTN applications to Handwriting Recognition (HWR), Automatic Speech Recognition (ASR) etc.   

## Quickstart

First [install](#installation) the python bindings.

The following is a minimal example of building two WFSAs with `gtn`, constructing a simple function on the graphs, and computing gradients. [![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/facebookresearch/gtn/blob/master/examples/notebooks/quick-start.ipynb)


```python
import gtn

# Make some graphs:
g1 = gtn.Graph()
g1.add_node(True)  # Add a start node
g1.add_node()  # Add an internal node
g1.add_node(False, True)  # Add an accepting node

# Add arcs with (src node, dst node, label):
g1.add_arc(0, 1, 1)
g1.add_arc(0, 1, 2)
g1.add_arc(1, 2, 1)
g1.add_arc(1, 2, 0)

g2 = gtn.Graph()
g2.add_node(True, True)
g2.add_arc(0, 0, 1)
g2.add_arc(0, 0, 0)

# Compute a function of the graphs:
intersection = gtn.intersect(g1, g2)
score = gtn.forward_score(intersection)

# Visualize the intersected graph:
gtn.draw(intersection, "intersection.pdf")

# Backprop:
gtn.backward(score)

# Print gradients of arc weights 
print(g1.grad().weights_to_list()) # [1.0, 0.0, 1.0, 0.0]
```

## Installation

### Requirements

- A C++ compiler with good C++14 support (e.g. g++ >= 5)
- `cmake` >= 3.5.1, and `make`

### Building C++ from source

First, clone the project:

```
git clone https://github.com/gtn-org/gtn.git && cd gtn
```

Create a build directory and run CMake and make:

```
mkdir -p build && cd build
cmake ..
make -j $(nproc)
```

Run tests with:

```
make test
```

Run `make install` to install.

### Python bindings from source

Setting up your environment:
```
conda create -n gtn_env
conda activate gtn_env
```

Required dependencies:
```
cd bindings/python
conda install -c conda-forge cxx-compiler libstdcxx-ng pybind11
```

If you want the CUDA version, install the nvcc too:

```
conda install nvidia::cuda-nvcc
```

Use one of the following commands for installation:

```
pip install .
```

Python binding tests can be run with `make test`, or with
```
python -m unittest discover bindings/python/test
```

Run a simple example:
```
python bindings/python/examples/simple_graph.py
```


### Citing this Repository

If you use the code in this repository, please cite:

Awni Hannun, Vineel Pratap, Jacob Kahn and Wei-Ning Hsu. Differentiable Weighted Finite-State Transducers. 
[arXiv 2010.01003](https://arxiv.org/abs/2010.01003), 2020.

```
@article{hannun2020dwfst,
  title={Differentiable Weighted Finite-State Transducers},
  author={Hannun, Awni and Pratap, Vineel and Kahn, Jacob and Hsu, Wei-Ning},
  journal={arXiv preprint arXiv:2010.01003},
  year={2020}
}
```

### License

GTN is licensed under a MIT license. See [LICENSE](LICENSE).
