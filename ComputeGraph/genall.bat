python graph_latency_study.py
python graph_with_higher_latency.py
python graph.py 

dot -Tpng -o Pictures\graph.png Pictures\graph.dot               
dot -Tpng -o Pictures\graph_with_higher_latency.png Pictures\graph_with_higher_latency.dot
dot -Tpng -o Pictures\graph_latency_study.png Pictures\graph_latency_study.dot
