#include <iostream>
#include <queue>
#include <stack>
#include <vector>

template<typename t>
class Tree {

private:

    struct node {
        t data; // Data must be of some numeric type that can be evaluated using > < or ==, possibly in the future we can make a key member of another template type such as string and convert it into an int and use that as the key
        node* leftChild;
        node* rightChild;
    };

    node* root;

public:

    Tree() : root{nullptr} {}

    ~Tree() {

        if (root == nullptr) {
            return;
        }
    
        std::stack<node*> s;
        node* current = root;
        s.push(root);

        while (!s.empty()) { // Postorder traversal where we delete node, code is not the best, definately better to just do level order traveral or some dfs traversal & insert all nodes into a vector and then delete them sqeuentially
        
            while (current != nullptr) { 

                if (current->rightChild != nullptr) {
                    s.push(current->rightChild);
                }
                if (current->leftChild != nullptr) {
                    s.push(current->leftChild);
                }

                current = current->leftChild;

            }

            current = s.top();

            if (current != nullptr && current->leftChild == nullptr) {
                s.pop();
                node* parent{ findParent(root, current) };

                if (parent->leftChild == current) {
                    parent->leftChild = nullptr;
                }
                else if (parent->rightChild == current) {
                    parent->rightChild = nullptr;
                }

                delete current;
                current = s.top();

            }
        }

        root = nullptr;

    }

    bool contains(const t& data, node* root) const {
    
        if (root == nullptr) {       
            return false;
        }
        else if (root->data == data){
            return true;
        }
        else if (root->data > data) {
            return contains(root->leftChild);
        }
        else if (root->data < data) {
            return contains(root->rightChild);
        }

    }

    void printInorder(node* root) const {
    
        if (root = nullptr) {
            return;
        }

        printInorder(root->leftChild);
        std::cout << root->data;
        printInorder(root->rightChild);

    }

    bool isComplete() { // If all except last level is full
    
        std::queue<node*> q{root};
        node* nextLevel{ nullptr };
        const int maxLevel{ findHeight(root) };
        int currentLevel{ 0 };

        while (!q.empty() && currentLevel != maxLevel-1) {

            node* currentItem{ q.front() };

            if (nextLevel == currentItem) {
                ++currentLevel;
                nextLevel == nullptr;
                if (currentLevel == maxLevel - 1) {
                    break;
                }
            }

            if (nextLevel == nullptr) { // To find start of next level

                if (currentItem->leftChild != nullptr) {
                    nextLevel = currentItem->leftChild;
                }
                else if (currentItem->rightChild != nullptr) {
                    nextLevel = currentItem->rightChild;
                }

            }

            if (currentItem->leftChild != nullptr) {
                q.push(currentItem->leftChild);
            }
            if (currentItem->rightChild != nullptr) {
                q.push(currentItem->rightChild);
            }

            q.pop();

        }
        
        int nodesRequired{ std::pow(2,currentLevel) }; // Number of nodes that would be on a certain level if tree was full 
            
        return nodesRequired == q.size();

    }

    node* findParent(node* current, node* target)  {
    
        if (current == nullptr || target == root ) { // If the item we are searching for doesnt exist in our tree, our tree is empty, or root is the node were searching for. You can check if root == nullptr, but you will always start with current as root, so just checking current is oaky
            return nullptr;
        }
        else if (current->leftChild == target || current->rightChild == target) {
            return current;
        }
        else if (current->data > target->data) {
            return findParent(root, current->leftChild);
        }
        else { // current->data < target->data
            return findParent(root, current->rightChild); 
        }

    }

    void remove(const t& data) {

        if (!contains(data) || root == nullptr) {
            return;
        }

        node* temp{ findNode(data) };
        node* parent{ findParent(root, temp) };

        if (temp == root) {

            if (size() == 1) {

                delete temp;
                root == nullptr;

            }

            else if (size() > 1) {

                std::vector<node*>* nodes = getNodes();
                int i{ 0 };
                while (nodes[i] != root) {
                
                    ++i;

                }
                nodes->erase(i);                   
                root = buildBalancedTree(nodes, 0, nodes->size()-1);
                delete nodes;
                delete temp;

            }
        }

        else if (temp->leftChild == nullptr && temp->rightChild == nullptr) { // If node we are deleting is a leaf 

            if (parent->leftChild == temp) { // To prevent dangling pointers
                parent->leftChild = nullptr;
            }
            else if (parent->rightChild == temp) {
                parent->rightChild = nullptr;
            }

            delete temp;

        }

        else { // If we are deleting a node that isnt root & also not a leaf - Bit messy, should probably be rewritten

            std::queue<node*> q;
            q.push(temp->leftChild);
            q.push(temp->rightChild);
            delete temp;

            while (!q.empty()) {

                temp = q.front();

                auto reinsert{ []() { 
                    node* location{ findNode(temp->data, root) };
                    location = temp;
                } };

                reinsert();

                if (temp->leftChild != nullptr)  {
                    q.push(temp->leftChild);
                }
                if (temp->rightChild != nullptr) {
                    q.push(temp->rightChild);
                }

                temp->leftChild = nullptr;
                temp->rightChild = nullptr;

                q.pop();

            }

        }

        if (!isBalanced()) {
            balanceTree();
        }
    }

    std::size_t size() const {

        std::size_t size{ 0 };
        std::queue<node*> q;

        q.insert(root);

        while (!q.empty()) {

            node* currentItem = q.front();
            q.pop();

            if (currentItem.leftChild != nullptr) 
                q.push(currentItem.leftChild);

            if (currentItem.rightChild != nullptr)     
                q.push(currentItem.rightChild);

            ++size;
        }

        return size;

    }

    node* findNode(const t& data, node* current) {
    
        if (current == nullptr || root->data == data) { // Short circuit first condition, as if you check second condition while its nullptr its throw an exception as you tried to deref a nullptr
            return current;
        }
        else if (root->data < data) {
            findNode(data, current->rightChild);
        }
        else if (root->data > data) {
            findNode(data, current->leftChild);
        }
    }

    void insert(const t& data) {
    
        if (contains(data)) {
            return;
        }
        
        node* address{ findNode(data, root) };
        address = createNode(data);

        if (!isBalanced()){
            balanceTree();
        }
             
    }

    node* buildBalancedTree(const std::vector<node*>& nodes, int start, int end) {

        if (start > end) {
            return nullptr;
        }

        int midpoint{ (start + end) / 2 };

        node* current{ nodes[midpoint] };

        current->leftChild  = buildBalancedTree(nodes, current->leftChild, start, midpoint-1);
        current->rightChild = buildBalancedTree(nodes, current->rightChild, midpoint+1, end); // Dont worry about a node which was previously not a leaf having child ptrs, as we will set the pointer to nullptr

        return current;

    }

    void balanceTree() {
    
        std::vector<node*>* nodesInorder = getNodes(); // Cant bracket initilize as by default, it will prioritize a member initilzer list.
        root = buildBalancedTree(nodesInorder, root, 0, nodesInorder->size() - 1);
        delete nodesInorder;

    }

    std::vector<node*>* getNodes() const {
    
        std::vector<node*>* nodes; // Probably better to insert into a linked list as its O(1), but we wont be able to get middle node using subscript later & we'll need to search sequentially if we wanted to get middle node on linked list, faster inserts for slower retrival of middle node!
        std::stack <node*> s;
        node* current = root;

        do {

            while (current != nullptr) {
                s.push(current);
                current = root->leftChild;
            }

            node* top{ s.top() };
            nodes.push_back(top);
            s.pop();

            if (top->rightChild != nullptr) {
                s.push(top->rightChild);
            }

        } while (!s.empty());

        return nodes;

    }

    bool isBalanced() const {

        if (size() > 3) {
        
            std::size_t leftHeight{ findHeight(root->leftChild) };
            std::size_t rightHeight{ findHeight(root->rightChild) };
            if (std::abs(leftHeight - rightHeight) > 1) {
                return false;
            }
        }

        return true;

    }

    std::size_t findHeight(node* root) const {
    
        if (root == nullptr) 
            return -1;
        else 
            return 1 + std::max(findHeight(root->leftChild), findHeight(root->rightChild));

    }

    node* createNode(const t& data) const {
        node* newNode{ new node{ data, nullptr, nullptr } };
        return node;
    }

    const node*& getRoot() const {
        return root;
    }

};


int main() {}


