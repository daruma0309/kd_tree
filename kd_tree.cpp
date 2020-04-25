#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#define D 2  // 2 dimensional
#define N 30 // data num
#define XAXIS 0
#define YAXIS 1
#define ZAXIS 2

struct Point {    
    int id;
    double pos[D]; // pos[0]=x, pos[1]=y, ...
};


struct Node {
    Point loc;
    
    Node *left;     // 左部分木
    Node *right;    // 右部分木
    
    int depth;      // 深さ
};

// kd-treeを生成
Node *make_kdtree(std::vector<Point> &plist, int left, int right, int depth) {
    if (left > right) { // 空ノード
        return NULL;
    }

    if (left == right){ // 葉
        Node *new_node = new Node;
        
        new_node->loc = plist[right];
        
        new_node->left = NULL;
        new_node->right = NULL;
        
        new_node->depth = depth;
        
        return new_node;
    }

    int axis = depth % D; // 分割軸を交互に選択
    
    Node *new_node = new Node;
    
    // 指定の軸でソート
    if (axis == XAXIS) {
        std::sort(plist.begin()+left, plist.begin()+right+1, 
        [](const Point &p1, const Point &p2){ return (p1.pos[0] < p2.pos[0]);});
    } else if (axis == YAXIS) {
        std::sort(plist.begin()+left, plist.begin()+right+1, 
        [](const Point &p1, const Point &p2){ return (p1.pos[1] < p2.pos[1]);});
    }
    
    int median = left + (right-left)/2;
    
    new_node->loc = plist[median];
    new_node->depth = depth;
    
    new_node->left = make_kdtree(plist, left, median-1, depth+1);       // 左部分木を生成
    new_node->right = make_kdtree(plist, median+1, right, depth +1);    // 右部分木を生成
    
    return new_node;
}

Node *nn_search(Point *query, Node *node, int depth) {
    if (node == NULL) return NULL;

    int axis = depth % D; // 分割軸を交互に選択

    // 左右どちらが近いか選択
    int flag;
    if (axis == XAXIS) {
        if ((node->loc).pos[0] > query->pos[0]) flag = 0;   // 左側
        else flag = 1;                                      // 右側
    } else {
        if ((node->loc).pos[1] > query->pos[1]) flag = 0;   // 左側
        else flag = 1;                                      // 右側
    }

    // 近い方の部分木を探索 node_n:nearer node
    Node *node_n;
    if (flag == 0) {
        node_n = nn_search(query, node->left, depth+1);
    } else {
        node_n = nn_search(query, node->right, depth+1);
    }

    // 葉ノードなら現在のノードを返す
    if (node_n == NULL) return node;
    
    // 現在のノードの距離 dis_cur:distance of current node
    double dis_cur = ((node->loc).pos[0] - query->pos[0]) * ((node->loc).pos[0] - query->pos[0])
                + ((node->loc).pos[1] - query->pos[1]) * ((node->loc).pos[1] - query->pos[1]);
    // 子ノードの距離 dis_c:distance of child node
    double dis_chi = ((node_n->loc).pos[0] - query->pos[0]) * ((node_n->loc).pos[0] - query->pos[0])
                + ((node_n->loc).pos[1] - query->pos[1]) * ((node_n->loc).pos[1] - query->pos[1]);

    // 近い方を返す
    if (dis_chi < dis_cur) return node_n;
    else return node;
}

// kd-treeを出力
void print_tree(Node *node) {
    if(node->right != NULL) print_tree(node->right);

    // 通りがけで出力
    for (int i = 0; i < node->depth; i++) {
        std::cout << "        ";
    }
    std::cout << " (" << (node->loc).pos[0] << "," << (node->loc).pos[1] << ")" << std::endl;

    if(node->left != NULL) print_tree(node->left);
}


int main(void) {
    std::vector<Point> plist;
    
    // 初期値を設定
    int cnt = 0;
    std::mt19937 mt(1); // 乱数生成器
    for (int i = 0; i < N; i++) {
        Point p;
        p.id = cnt++;
        p.pos[0] = (mt() % 100) / 10.0; // 0 ~ 9.9までの乱数
        p.pos[1] = (mt() % 100) / 10.0;

        plist.push_back(p);
        std::cout << p.id << " (" << p.pos[0] << "," << p.pos[1] << ")" << std::endl;
    }

    // kd-treeを生成
    Node *root = make_kdtree(plist, 0, N-1, 0);
    std::cout << "[made kd-tree]" << std::endl;
    print_tree(root);

    // 最近傍探索
    Point q;
    q.id = 11; q.pos[0] = 3.0; q.pos[1] = 3.0;
    Node *node = nn_search(&q, root, 0);
    std::cout << "[nn_search]" << std::endl;
    std::cout << "query : (" << q.pos[0] << "," << q.pos[1] << ")" << std::endl;
    std::cout << "nearer : (" << (node->loc).pos[0] << "," << (node->loc).pos[1] << ")" << std::endl;

    return 0 ;
}
