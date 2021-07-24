#include <iostream>
#include <vector>
#include <string>
using std::vector;
using std::string;
#include <unordered_map>
#include <utility>
using std::pair;

#include "bit208.hpp"
using bits = bit208;

#include <algorithm>
using std::min;

#include <chrono>
#include <time.h>

vector<bits> elements;
vector<bits> maxBitsFrom;
vector<vector<pair<bits,bits>>> requiredBitsFrom;
bits endCondition;
unsigned short unique;            // amount of unique bits necessary
unsigned short curMin;            // initialized at total (solution must be guaranteed), decreases only on valid totals


// comb represents the current combination of elements, total is the current total, cur is the current element to either include or exclude
unsigned short getMin(bits comb, unsigned short total, vector<bits>::iterator cur){
    if(comb == endCondition){ return total; }
    if(total >= curMin - 1){ return curMin; }           // back track
    if(cur == elements.end()){ return curMin; }         // very end case

    if(comb.CountComb(maxBitsFrom[cur-elements.begin()]) < unique){ return curMin; }           // branch and bound

    while(!comb.Contributes((*cur))){       // only consider non-trivial inclusions
        ++cur;                              // assuming a solution exists
    }

    // Include required unique bits at each branching level
    if(!requiredBitsFrom[cur-elements.begin()].empty()){
        auto v = requiredBitsFrom[cur-elements.begin()];
        unsigned short unique=0;
        for(const pair<bits,bits> &p : v){
            if(comb.Contributes(p.second)){       // if currently missing the unique bits, include the element that has them
                comb |= p.first;
                unique++;
            }
        }
        if(unique > 0){         // Need to recheck end conditions
            total += unique;
            if(comb == endCondition){ return total; }
            if(total >= curMin - 1){ return curMin; }       // back track
            while(!comb.Contributes((*cur))){               // make sure next inclusion is non-trivial
                ++cur;
            }
        }
    }

    if(total == curMin - 2){                            // another backtracking push
        for(auto itr=cur;itr!=elements.end();itr++){
            if((comb|*itr) == endCondition){
                return total+1;    // if a single element meets the end condition, return 1 below
            }
        }
        return curMin;              // if no such element exists, backtrack
    }

    ++cur;

    // By splitting the function calls, curMin is updated before the second call making it easier to backtrack
    curMin = min(curMin, getMin(comb|(*(cur-1)),total+1,cur));      // include current element
    curMin = min(curMin, getMin(comb,total,cur));                   // exclude current element

    return curMin;
}

// This allows the inclusion of elements with high range first which makes initial recursive calls closer to end condition
bool SortBits (const bits& b1, const bits& b2){ return b1.count() > b2.count(); }

void SortElementsByContribution();
void RemoveNonContributors();
void FillReqBitsTable();

int main(){
    auto start = std::chrono::high_resolution_clock::now();
    size_t N, K;
    std::cin >> N >> K;
    unique = K;

    std::unordered_map<string,bits> translate;

    string s;
    bits b;
    for(unsigned short i=0;i<K;i++){
        std::cin >> s;
        translate.insert({s,b.reset().set(i)});     // each category is represented by a unique bit
    }

    for(unsigned int i=0;i<N;i++){
        std::cin >> K;
        b.reset();
        for(unsigned int j=0;j<K;j++){
            std::cin >> s;
            b |= translate.at(s);                   // an element's contribution is the union of their bits
        }
        elements.push_back(b);
    }

    // Optimize by having those that contribute more get considered first
    // this makes helps find a low curMin early which allows for more backtracking
    // TODO: There are more involved yet optimal ways to sort
    std::sort(elements.begin(), elements.end(), SortBits);    // sort by individual bit count (makes list semi-sorted before insertion sort)
    SortElementsByContribution();                             // sort by contribution with respect to sorted subsection (with insertion sort)

    RemoveNonContributors();

    // NOTE: List is immutable from this point onwards

    // Fill a table that stores a list of elements who have unique bits and therefore must be included with respect to the subset [ elements[index], elements[end] ]
    FillReqBitsTable();

    // This is used for back tracking lookups in constant time
    b.reset();
    for(int i=int(elements.size()-1); i>=0; i--){ maxBitsFrom.push_back(b |= elements[i]); }
    std::reverse(maxBitsFrom.begin(), maxBitsFrom.end());

    endCondition = maxBitsFrom[0];
    curMin = unique;

    std::cout << "Starting branching with n=" << elements.size() << '\n' <<
        getMin(0,0,elements.begin()) << '\n';

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << (std::chrono::duration_cast<std::chrono::milliseconds>(end-start)).count()/1000.0 << " seconds\n";
    return 0;
}

void SortElementsByContribution(){
    bits cur(elements[0]);
    for(int i=1; i<int(elements.size()); i++){
        for(int j=i;j>0 && elements[j].CountComb(cur) < elements[j-1].CountComb(cur);j--){
            std::swap(elements[j], elements[j-1]);
        }
        cur |= elements[i-1];
    }
}

// Deleting any element which is a subset of another
void RemoveNonContributors(){
    vector<int> toDelete;
    for(int i=int(elements.size()-1); i>=0; i--){       // more likely to be low contribution, starts at the end
        for(int j=0;j<i;j++){                           // more likely to have overlap since higher contribution, starts at beginning
            if(!elements[j].Contributes(elements[i])){
                toDelete.push_back(i);
                break;
            }
        }
    }
    std::cout << "Deleting " << toDelete.size() << '\n';
    for(int i : toDelete){ elements.erase(elements.begin()+i); }
}

void FillReqBitsTable(){
    bits zero;
    for(int i=0; i<int(elements.size()); i++){
        vector<pair<bits,bits>> reqs;       // first is the element's bits, second is the unique bits the elements has
        for(int j=i; j<int(elements.size()); j++){
            bits cur = elements[j];
            for(int k=i; k<int(elements.size()); k++){
                if(k==j){ continue; }
                cur &= ~elements[k];          // mask off all other bits in the subsection of the list
                if(cur == zero){ break; }
            }
            if(cur != zero){ reqs.emplace_back(elements[j], cur); }   // if after masking there are bits left, they need to be stored
        }
        requiredBitsFrom.push_back(reqs);
    }
}
