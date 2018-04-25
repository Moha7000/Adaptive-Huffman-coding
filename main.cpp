/*
Adaptive Huffman Algorithm 
*/

#include<bits/stdc++.h>
using namespace std;

#define not_symbol -1   // for internal nodes
map<char,int> char_set;
map<char,string> codes;
int m, e, r;


/*
structure of each node
*/

struct node{
    char symbol;
    int order, weight;
    bool nyt, root;
    node *left, *right, *parent;
};

map<char , node*> Symbols;

/*
Creating Tree with the root
returns pointer to the root node
*/
node* create_Tree(){
    node* tree   = new node;
    tree->nyt    = 1;
    tree->root   = 1;
    tree->symbol = not_symbol;
    tree->weight = 0;
    tree->order  = m * 2 - 1;
    tree->parent = NULL;
    tree->left   = NULL;
    tree->right  = NULL;

    return tree;
}
/*
Obtain the node corresponding to it's symbol in the tree
Symbols : global map for all symbols
returns : pointer to tree node corresponding to the passed symbol, if the node does't exist , returns null
*/
node* get_symbol_position_from_tree(char symbol){
    return Symbols[symbol];
}

/*
Creates new child in the tree
returns created node
*/
node* add_new_child(node* parent, bool is_nyt, bool is_root, char symbol, int weight, int order){
    node* temp    = new node;
    temp->nyt     = is_nyt;
    temp->root    = is_root;
    temp->parent  = parent;
    temp->left    = temp->right = NULL;
    temp->weight  = weight;
    temp->order   = order;
    temp->symbol  = symbol;
    return temp;
}


/*
Add new Symbol to the tree and add it's node in the global map Symbols
parameters : symbol
             Reference to the nyt node
returns the internal node that symbol is on it's right and new nyt is on it's left.
*/
node* add_symbol(char symbol, node ** nyt){
    node* left_node  = add_new_child(*nyt, 1, 0, not_symbol, 0, (*nyt)->order - 2);
    node* right_node = add_new_child(*nyt, 0, 0, symbol    , 1, (*nyt)->order - 1);

    node* old_nyt = *nyt;
    // convert old nyt node to an internal node
    (*nyt)->nyt = 0;
    (*nyt)->symbol = not_symbol;
    (*nyt)->left = left_node;
    (*nyt)->right = right_node;

    // new nyt node
    *nyt = left_node;
    Symbols[symbol] = right_node;
    return old_nyt;
}


void Swap(node *x, node *y){

    swap(x->order , y->order);

    x->parent->left = y  ;
    y->parent->right = x;


}
/*
Check if swap needed to be done to balance the tree
take current node as parameter ,
returns the right node should be swapped with the current node if existed and NULL if no swap needed
*/
node* Check_for_swap(node* current_node){
    node *left = current_node->parent->left;
    node *right = current_node->parent->right;

    if(left == current_node){
        if(left->weight == right->weight)
            return right;
    }
    return NULL;
}
/*
Returns string contain the code corresponding to the given node ( NYT or Symbol node)
*/
string Find_node_path(node *n , node *root){
    string path = "";
    while(n != root){
        if(n->parent->left == n)
            path += "0";
        else
            path += "1";
        //cout << n->weight << endl;
        n = n->parent;
    }
    reverse(path.begin(),path.end());
    return path;
}


/*
update_tree:
update tree weight after inserting new symbol and do swap if necessary
*/
void update_Tree(node *curr_node,node *root){
    while(!curr_node->root){
        // swap take place here !

        node *replace_node = Check_for_swap(curr_node);

        if(replace_node){
            Swap(curr_node, replace_node);
            //cout << "Swap happens between " << replace_node->symbol << " and "  << curr_node->symbol << endl ;
            if(curr_node->symbol != -1)
                codes[curr_node->symbol] = Find_node_path(curr_node,root);
            else if(replace_node->symbol != -1)
                codes[replace_node->symbol] = Find_node_path(replace_node,root);

        }

        (curr_node->weight)++;  // increase weight

        curr_node = curr_node->parent; // go upper
    }
    // here current node pointing to the root
    (curr_node->weight)++;
}

/*
convert a binary value into it's string representation with specific number of bits
*/
string convert(int value , int numberOfBits){
    string ret = "";
    for(int i = 0 ; i<numberOfBits ; i++){
            if(value&1)
                ret += "1";
            else
                ret += "0";
            value/=2;
    }
    reverse(ret.begin() , ret.end());
    return ret;
}
/*
obtain the fixed code for a letter according to it's index in character set
*/
string get_fixed_code(int k , int e, int r ){
    string binary ;

    if ( k <= 2 * r && k >= 1 ){
      //   send  (e+1) bits binary representation of (k-1);
        binary = convert(k-1, e+1); //to binary
     }
      else if( k > 2 * r ){
      //   send  (e) bits binary representation of (k-r-1);
        binary = convert(k - r - 1 , e); //to binary
      }
      return binary;
}


string encode (string message){
    node *root = create_Tree();
    node *nyt  = root;
    node *temp;
    char letter ;
    string encoded = "";
    for (int i = 0 ; i < message.length() ; i++){
        letter = message[i];
        node *exist = get_symbol_position_from_tree(letter);
        if(!exist ){
            encoded += Find_node_path(nyt, root);
            encoded += get_fixed_code(char_set[letter], e, r);
            //encoded += " ";
            temp = add_symbol(letter , &nyt);
            update_Tree(temp, root);
            //cout << letter << " code " <<  Find_node_path(Symbols[letter],root) << endl ;
        }
        else {
            encoded += Find_node_path(exist, root);
            //encoded += " ";

            update_Tree(exist, root);
            //cout << letter << " code " <<  Find_node_path(Symbols[letter],root) << endl ;

        }
        //encoded += "   ";
    }
    codes.clear();
    Symbols.clear();
    return encoded;
}
/*
convert a binary number represented in a string to value (int)
*/
int convert_to_int(string x){
    int n = 0 , j = -1 , l = x.length();
    while (++j < l)
        n = (n << 1) + (x[j] == '0' ? 0 : 1);
    return n;
}
/*
obtain character index from character set
*/
char get_letter(int index){
    for(auto x:char_set){
        if(index == x.second)
            return x.first;
    }
}
/*
decode take a binary msg and returns the original msg
*/
string Decode(string message, int e, int r){
    string decoded = "";
    string buffer = "";
    string nyt_code = "";
    string letter_code ;

    int cursor = 0;
    char letter;

    node *root = create_Tree();
    node *nyt = root;
    node *temp;

    codes.clear();
    Symbols.clear();
    while(cursor < message.length()){
        nyt_code = Find_node_path(nyt, root);
        //cout << "nyt_code  "  <<   nyt_code << endl ;
        //char is not in the tree and it's code consists of nyt code + fixed code
        if(nyt_code == message.substr(cursor,nyt_code.length()) || root == nyt){
            cursor += nyt_code.length();
            buffer = message.substr(cursor,e);
            cursor += e;
            int index = convert_to_int(buffer);
            if(index < r){
                buffer += message[cursor];
                ++cursor;
                index = convert_to_int(buffer);
                ++index;
            }
            else{
                index += (r + 1);
            }
            letter = get_letter(index);
            //cout << letter << endl ;

            temp = add_symbol(letter, &nyt);
            update_Tree(temp, root);
            letter_code = Find_node_path(Symbols[letter], root);
            codes[letter] = letter_code;
        }
        // char is already decoded once and is in the tree
        else{
            char l; string c;
            for(auto x:codes){
                l = x.first , c = x.second;
                //cout << l << "  " << c << endl ;
                if(c == message.substr(cursor,c.length())){
                    letter = l;
                    cursor += c.length();
                    node *t =get_symbol_position_from_tree(letter);
                    update_Tree(t, root);
                    codes[letter] = Find_node_path(t,root);
                    break;
                }
            }
        }
        // update codes of already decoded letters
        for(auto x:codes){
            node *t = get_symbol_position_from_tree(x.first);
            x.second = Find_node_path(t,root);
            codes[x.first] = x.second;

        }
        decoded += letter;

    }
    return decoded ;
}
int main()
{
    cout << "\t\t\t-----Adaptive Huffman Algorithm------" << endl << endl;
    cout << "Please write down your character set in input file -Set_of_character-" << endl  << endl ;
    ifstream input_file;
    string line, input_set;

    // preparing character set
    input_file.open("Set_of_characters.txt");
    if(input_file.is_open()){
        while(getline(input_file, line))
            input_set += line;
        input_file.close();
    }
    else{
        cout <<
        "Unable to open input file\n";
        return 0;
    }
    // mapping every char in character set to it's index in global map char_set
    int cnt = 1;
    for(int i = 0; i < input_set.length();++i){
        char letter = input_set[i] ;
        if(letter != ' '){
            char_set[letter] = cnt;
            ++cnt;
        }
        // getting values m , e and r for calculations
        m = cnt - 1;
        e = log2(m * 1.0);
        r = m - pow(2,e);


    }

    ifstream input_msg;
    ofstream output;
    int choice , confirm ;
    string msg = "";
    int filess;
    cout << "enter 1 to work using files or enter 2 to work in console"<<endl ;
    cin >> filess;
    if (filess == 1){
        while(1){
        msg = "";
        //string line = "";
        cout << "To Encode message Enter 1" << endl;
        cout << "To Decode Encoded message Enter 2" << endl ;
        cout << "To Exit Enter 0" << endl ;
        cin >> choice ;
        if(choice == 0)
            return cout << "Thanks for using this program\n" ,0;

        cout << endl << "Write down your message into input_message file -input_message.txt- and then select your choice" << endl << endl ;
        cout << endl << "Enter 1 when you're file is ready and to exit enter 0" << endl ;
        cin >> confirm;

        if(!confirm){
            return cout << "Thanks for using this program\n" ,0;
        }
        if(choice == 1){
            input_msg.open("input_message.txt");
            while(getline(input_msg, line))
                msg += line;
            input_msg.close();
            //cout << msg << endl ;
            string Encoded = encode(msg);
            output.open("output.txt");
            output << Encoded ;
            output.close();
            cout << endl << "Your message has been encoded successfully !\n check your output file please!" << endl  << endl ;
        }
        else if(choice == 2){
            input_msg.open("input_message.txt");
            while(getline(input_msg, line))
                msg += line;
            input_msg.close();
            string Decoded = Decode(msg, e, r);
            output.open("output.txt");
            output << Decoded  ;
            output.close();
            cout << endl << "Your message has been decoded successfully !\n check your output file please!" << endl  << endl ;

        }

        else{
            cout << "Error please choose 1 or 2 " << endl ;
            continue;
        }
        }
    }
    else if(filess== 2){
        while(1){
            msg = "";
            //string line = "";
            cout << "To Encode message Enter 1" << endl;
            cout << "To Decode Encoded message Enter 2" << endl ;
            cout << "To Exit Enter 0" << endl ;
            cin >> choice ;
            if(choice == 0)
                return cout << "Thanks for using this program\n" ,0;


            if(choice == 1){
                cout << "Enter your message " << endl << endl ;
                cin >> msg;
                //cout << msg << endl ;
                string Encoded = encode(msg);

                cout << endl << "Your message has been encoded successfully !\n" << endl  << endl ;
                cout << Encoded << endl  << endl ;
            }
            else if(choice == 2){
                cout << "Enter your message " << endl << endl ;
                cin >> msg ;
                string Decoded = Decode(msg, e, r);
                cout << endl << "Your message has been decoded successfully !\n" << endl  << endl ;
                cout << Decoded << endl << endl ;
            }

            else{
                cout << "Error please choose 1 or 2 " << endl ;
                continue;
            }

        }
    }
    return 0;
}
// different character sets
//abcdefghijklmnopqrstuvwxyz
//0123456
//ƒηΩӨβ∞

