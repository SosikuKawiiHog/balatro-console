#include <iostream>
#include <utility>
#include <vector>
#include <random>
#include <algorithm>
#include <map>
#include <list>
#include <memory>

using namespace std;

class Card{
    pair<string, int> _card;
public:
    Card(const string& rank, int suit): _card(rank, suit) {}
    ~Card() = default;
    string get_rank() const{
        return _card.first;
    }
    int get_suit() const{
        return _card.second;
    }
    void set_rank(string rank){
        _card.first = std::move(rank);
    }
    void set_suit(int suit){
        _card.second = suit;
    }
    string suit_symbol() const{
        switch (_card.second){
            case 0: return "♠";
            case 1: return "♣";
            case 2: return "♥";
            case 3: return "♦";
        }
    }
    void print() const{
        cout << suit_symbol() << _card.first;
    }
};

enum class PokerHand{
    HighCard,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    Straight,
    Flush,
    FullHouse,
    FourOfAKind,
    StraightFlush,
    RoyalFlush
};

class Hand{
    vector<Card> _hand;
    vector<Card> _selected;
    int _max_select = 5;
public:
    Hand() : _hand(), _selected() {}
    Hand(vector<Card> hand){
        _selected.clear();
        _hand.reserve(hand.size());
        for(const auto& cards: hand){
            _hand.push_back(cards);
        }
    }

    void append(vector<Card> hand){
        _selected.clear();
        for(const auto& cards: hand){
            _hand.push_back(cards);
        }
    }

    PokerHand evaluate_selected(){
        vector<string> ranks;
        for(const auto& cards: _selected){
            ranks.push_back(cards.get_rank());
        }
        map<string, int> ranks_weight = {{"2", 2},{"3",3}, {"4",4}
                ,{"5",5}, {"6",6}, {"7",7}, {"8",8}, {"9",9},
                {"10", 10}, {"J",11},{"Q", 12}, {"K",13}, {"A",14}}; //БЛЯТЬ СИЛАЙОН ТЫ КАК ЭТО РАСПИДОРАСИЛ
        ::sort(ranks.begin(),ranks.end(),[&ranks_weight](const string& a, const string& b){
            return ranks_weight[a] < ranks_weight[b];
        });
        vector<int> numeric_ranks;
        for(const auto& card : ranks){
            auto it = ranks_weight.find(card);
            if(it != ranks_weight.end()){
                numeric_ranks.push_back(it->second);
            }
        }

        map<int, size_t> rank_counts;
        for(const auto& card: numeric_ranks){
            rank_counts[card]++;
        }
//        for(const auto& pair: rank_counts){
//            cout << "CHECK";
//            cout << endl << "pairfirst: " << pair.first << " pairsecond: " << pair.second << endl;
//        }
        bool isFull = false;
        bool isStraight = false;
        bool isFlush = false;
        bool isTwoPair = false;
        if(_selected.size() > 4){
            for(int i = 1; i < _selected.size()-1; i++){
                if(_selected.at(i).get_suit() == _selected.at(i-1).get_suit()){
                    isFlush = true;
                }
                else{
                    isFlush = false;
                    break;
                }
            }

            for(int i = 1; i < _selected.size()-1; i++){
                if(numeric_ranks.at(i) - numeric_ranks.at(i-1) == 1){
                    isStraight = true;
                }
                else{
                    isStraight = false;
                    break;
                }
            }

            if(isStraight && isFlush) return PokerHand::RoyalFlush;
            if(numeric_ranks.front() == 14) return PokerHand::StraightFlush;

            if(rank_counts.size() == 2){
                for(const auto& pair: rank_counts){
                    if(pair.second == 2 || pair.second == 3){
                        isFull = true;
                    }
                }
            }
        }
        if(_selected.size() > 3){
            if(rank_counts.size() >= 1){ //каре
                for(const auto& pair: rank_counts){
                    if(pair.second == 4){
                        return PokerHand::FourOfAKind;
                    }
                }
            }
            if(isFull) return PokerHand::FullHouse;
            if(isFlush) return PokerHand::Flush;
            if(isStraight) return PokerHand::Straight;
            int counter = 0;
            for(const auto& pair: rank_counts){
                if(pair.second == 2){
                    counter++;
                }
            }
            if(counter == 2){
                return PokerHand::TwoPair;
            }
        }
        if(_selected.size() > 2){
            for(const auto& pair: rank_counts){
                if(pair.second == 3){
                    return PokerHand::ThreeOfAKind;
                }
            }
        }
        if(_selected.size() > 1){
            for(const auto& pair: rank_counts){
                if(pair.second == 2){
                    return PokerHand::OnePair;
                }
            }
        }
        return PokerHand::HighCard;
    }

    void select(size_t index){
        if(_selected.size() == _max_select) return;
        if(_hand.size() > index){
            _selected.push_back(_hand.at(index));
            _hand.erase(_hand.begin() + index);
            define_hand();
        }
        else{
            cout << "Illegal pick" << endl;
        }
    }

    void unselect(size_t index){
        if(_selected.size() == 0) return;
        if(_selected.size() > index){
            _hand.push_back(_selected.at(index));
            _selected.erase(_selected.begin() + index);
            define_hand();
        }
        else{
            cout << "Illegal pick" << endl;
        }
    }

    void define_hand(){
        PokerHand ph = evaluate_selected();
        int val = static_cast<int>(ph);

        cout << "Current hand: ";
        switch(val){
            case 0: cout << "High Card"; break;
            case 1: cout << "Pair"; break;
            case 2: cout << "Two Pair"; break;
            case 3: cout << "Three of a kind"; break;
            case 4: cout << "Straight"; break;
            case 5: cout << "Flush"; break;
            case 6: cout << "Fullhouse"; break;
            case 7: cout << "Four of a kind"; break;
            case 8: cout << "StraightFlush"; break;
            case 9: cout << "RoyalFlush"; break;

        }
        cout << endl;
    }

    void sort_by_suit(){
        if(!_selected.empty()) return;
        ::sort(_hand.begin(), _hand.end(), [](auto &left, auto &right) {
            return left.get_suit() < right.get_suit();
        });
    }

    void sort_by_rank(){
        if(!_selected.empty()) return;
        vector<string> ranks;
        for(const auto& cards: _selected){
            ranks.push_back(cards.get_rank());
        }
        map<string, int> ranks_weight = {{"2", 2},{"3",3}, {"4",4}
                ,{"5",5}, {"6",6}, {"7",7}, {"8",8}, {"9",9},
                                         {"10", 10}, {"J",11},{"Q", 12}, {"K",13}, {"A",14}};
        ::sort(_hand.begin(), _hand.end(), [&ranks_weight](auto &left, auto &right) {
            return ranks_weight[left.get_rank()] < ranks_weight[right.get_rank()];
        });
        reverse(_hand.begin(), _hand.end());
    }

    vector<Card>& get_selected(){
        return _selected;
    }

    void print(){
        cout << "Selected: ";
        for(const auto& cards: _selected){
            cards.print();
            cout << " ";
        }
        cout << endl;
        cout << endl <<  "Hand: ";
        for(const auto& cards: _hand){
            cards.print();
            cout << " ";
        }
        cout << endl;
    }
};


class Effect{
public:
    virtual void apply_effect(Hand& hand, size_t& chips, size_t& mult) const = 0;
    virtual string get_description() const = 0;
};

class SimpleMultEffect: public Effect{
    void apply_effect(Hand& hand, size_t& chips, size_t& mult) const override{
        mult += 4;
    }
    string get_description() const{
        return "+4 mult";
    }
};

class PairChips: public Effect{
    void apply_effect(Hand& hand, size_t& chips, size_t& mult) const override{
        auto ph = hand.evaluate_selected();
        if(ph == PokerHand::OnePair || ph == PokerHand::TwoPair || ph == PokerHand::ThreeOfAKind || ph == PokerHand::FullHouse || ph == PokerHand::FourOfAKind){
            chips += 50;
        }
    }
    string get_description() const override{
        return "+50 chips if contains a pair";
    }
};

class BloodGem: public Effect{
    void apply_effect(Hand& hand, size_t& chips, size_t& mult) const override{
        for(const auto& cards: hand.get_selected()){
            if(cards.get_suit() == 2){
                mult *= 2;
            }
        }
    }
    string get_description() const override{
        return "*2 mult for each Heart card";
    }
};

class ChipsXMult: public Effect{
    void apply_effect(Hand& hand, size_t& chips, size_t& mult) const override{
        double average = (chips + mult) / 2.;
        chips = average;
        mult = average;
    }
    string get_description() const override{
        return "Balances chips and mult";
    }
};

enum class Rarity{
    common,
    uncommon,
    rare,
    legendary,
};

class Joker{
    string _name;
    shared_ptr<Effect> _effect;
    int _price;
    Rarity _rarity;
public:
    Joker(string name, unique_ptr<Effect> effect,int price, Rarity rarity): _name(name), _effect(std::move(effect)), _price(price), _rarity(rarity) {}
    void apply_effect(Hand& hand, size_t& chips, size_t& mult) const{
        _effect->apply_effect(hand, chips, mult);
    }
    int get_price() const {return _price; }
    Rarity get_rarity() { return _rarity; }
    string get_description() { return _effect->get_description(); }
    void print() const {
        switch(_rarity){
            case Rarity::common: cout << "Common: "; break;
            case Rarity::uncommon: cout << "Uncommon: "; break;
            case Rarity::rare: cout << "Rare: "; break;
            case Rarity::legendary: cout << "Legendary: "; break;
        }
        cout << _name << " $" << _price << endl;
    }
};

class JokerManager{
    vector<unique_ptr<Joker>> _all_jokers;
    mt19937 _rng;

    const map<Rarity, int> _rarity_weights ={
            {Rarity::common, 60},
            {Rarity::uncommon, 20},
            {Rarity::rare, 15},
            {Rarity::legendary, 5}
    };
public:
    JokerManager(): _rng(random_device{}()){
        initialize_jokers();
    }
    void initialize_jokers(){
        _all_jokers.emplace_back(make_unique<Joker>("Joker", make_unique<SimpleMultEffect>(),2, Rarity::common));
        _all_jokers.emplace_back(make_unique<Joker>("Couples", make_unique<PairChips>(),3, Rarity::uncommon));
        _all_jokers.emplace_back(make_unique<Joker>("Bloobonr", make_unique<BloodGem>(),4, Rarity::rare));
        _all_jokers.emplace_back(make_unique<Joker>("NotEldenRing", make_unique<ChipsXMult>(),7,Rarity::legendary));
    };

    vector<const Joker*> get_random_jokers(int count){
        vector<const Joker*> res;
        vector<unique_ptr<Joker>> temp;

        vector<Rarity> rarity_pool;
        for(const auto& [rarity, weight]: _rarity_weights){
            for(int i = 0; i < weight; i++){
                rarity_pool.push_back(rarity);
            }
        }
        ::shuffle(rarity_pool.begin(), rarity_pool.end(), _rng);
        for(int i = 0; i < count; i++){
            Rarity selected_rarity = rarity_pool[i % rarity_pool.size()];
            vector<Joker*> matching_jokers;
            for(const auto& joker: _all_jokers){
                if(joker->get_rarity() == selected_rarity){
                    matching_jokers.push_back(joker.get());
                }
            }
            if(!matching_jokers.empty()){
                uniform_int_distribution<size_t> dist(0, matching_jokers.size()-1);
                res.push_back(matching_jokers[dist(_rng)]);
            }
        }
        return res;
    }
};

class Deck{
    vector<Card> _deck;
    vector<Card> _used_cards;
    random_device _rd;
    mt19937 _gen;
    Hand _curr_hand;
    int _cards = 52;
    int _play_count = 5;
    int _discard_count = 3;
public:
    Deck() : _gen(_rd()), _curr_hand(), _used_cards() {
        vector<string> ranks = {"A","K","Q","J","10","9","8","7","6","5","4","3","2"};
        //vector<string> ranks = {"K","K","K","K","10","10","10","10","10","10","10","10","10"};
        vector<int> suits = {0,1,2,3}; //0 пики, 1 трефы, 2 черви, 3 бубны
        for(const auto& rank : ranks){
            for(const auto& suit: suits){
                _deck.emplace_back(rank, suit);
            }
        }
        shuffle(_deck.begin(),_deck.end(), _gen);
    }
    vector<Card>& get_deck() { return _deck; }
    Hand& get_hand() { return _curr_hand; }
    int get_cards_amount() const  { return _cards; }
    int get_play_count() const { return _play_count;}
    int get_discard_count() const { return _discard_count; }


    void make_hand(unsigned int amount){
        vector<Card> drawn_hand;
        drawn_hand.reserve(amount);
        for(int i = 0; i < amount; i++){
            drawn_hand.push_back(_deck.back());
            _deck.pop_back();
        }
        _curr_hand = drawn_hand;
    }

    void discard(){
        if(_discard_count != 0 && !get_hand().get_selected().empty()){
            vector<Card> drawn_hand;
            drawn_hand.reserve(_curr_hand.get_selected().size());
            for(int i = 0; i < _curr_hand.get_selected().size(); i++){
                drawn_hand.push_back(_deck.back());
                _deck.pop_back();
            }
            for(const auto& used: _curr_hand.get_selected()){
                _used_cards.emplace_back(used);
            }
            _curr_hand.append(drawn_hand);
            _discard_count--;
        }
    }

    size_t play(const vector<unique_ptr<Joker>>& active){
        if(_play_count != 0 && !get_hand().get_selected().empty()){
            vector<string> ranks;
            for(const auto& cards: get_hand().get_selected()){
                ranks.push_back(cards.get_rank());
            }
            map<string, int> ranks_weight = {{"2", 2},{"3",3}, {"4",4}
                    ,{"5",5}, {"6",6}, {"7",7}, {"8",8}, {"9",9},
                                             {"10", 10}, {"J",11},{"Q", 12}, {"K",13}, {"A",14}}; //БЛЯТЬ СИЛАЙОН ТЫ КАК ЭТО РАСПИДОРАСИЛ
            ::sort(ranks.begin(),ranks.end(),[&ranks_weight](const string& a, const string& b){
                return ranks_weight[a] < ranks_weight[b];
            });
            vector<int> numeric_ranks;
            for(const auto& card : ranks){
                auto it = ranks_weight.find(card);
                if(it != ranks_weight.end()){
                    numeric_ranks.push_back(it->second);
                }
            }

            map<int, size_t> rank_counts;
            for(const auto& card: numeric_ranks){
                rank_counts[card]++;
            }
//            for(const auto& pair: rank_counts){
//                cout << "CHECK";
//                cout << endl << "pairfirst: " << pair.first << " pairsecond: " << pair.second << endl;
//            }
            size_t chips = 0;
            size_t mult = 0;
            PokerHand ph = _curr_hand.evaluate_selected();
            int val = static_cast<int>(ph);
            switch(val) {
                case 0: {
                    chips = 5, mult = 1;
                    auto max_elem = *rank_counts.rbegin();
                    chips += max_elem.first;
                    break;
                }
                case 1: {
                    chips = 10, mult = 2;
                    for (const auto &pairs: rank_counts) {
                        if (pairs.second == 2) {
                            chips += pairs.first * 2;
                        }
                    }
                    break;
                }
                case 2: {
                    chips = 20, mult = 2;
                    for (const auto &pairs: rank_counts) {
                        if (pairs.second == 2) {
                            chips += pairs.first * 2;
                        }
                    }
                    break;
                }
                case 3: {
                    chips = 30, mult = 3;
                    for (const auto &pairs: rank_counts) {
                        if (pairs.second == 3) {
                            chips += pairs.first;
                        }
                    }
                    break;
                }
                case 4: {
                    chips = 30, mult = 4;
                    for (const auto &pairs: rank_counts) {
                        chips += pairs.first;
                    }
                    break;
                }
                case 5: {
                    chips = 35, mult = 4;
                    for (const auto &pairs: rank_counts) {
                        chips += pairs.first * pairs.second;
                    }
                    break;
                }
                case 6: {
                    chips = 40, mult = 4;
                    for (const auto &pairs: rank_counts) {
                        if(pairs.second == 4){
                            chips += pairs.first * pairs.second;
                        }
                    }
                    break;
                }
                case 7: {
                    chips = 60, mult = 7;
                    for (const auto &pairs: rank_counts) {
                        chips += pairs.first * pairs.second;
                    }
                    break;
                }
                case 8: {
                    chips = 100, mult = 8;
                    for (const auto &pairs: rank_counts) {
                        chips += pairs.first * pairs.second;
                    }
                    break;
                }
            }
            for(const auto& joker: active){
                joker->apply_effect(_curr_hand,chips, mult);
            }
            vector<Card> drawn_hand;
            drawn_hand.reserve(_curr_hand.get_selected().size());
            for(int i = 0; i < _curr_hand.get_selected().size(); i++){
                drawn_hand.push_back(_deck.back());
                _deck.pop_back();
            }
            for(const auto& used: _curr_hand.get_selected()){
                _used_cards.emplace_back(used);
            }
            _curr_hand.append(drawn_hand);
            _play_count--;
            return chips*mult;
        }
        return 0;
    }
    void refill_deck(){
        vector<Card> used_and_curr;
        used_and_curr.insert(used_and_curr.end(), _deck.begin(), _deck.end());
        used_and_curr.insert(used_and_curr.end(), _used_cards.begin(), _used_cards.end());
        _deck = used_and_curr;
        std::shuffle(_deck.begin(), _deck.end(), _gen);
        _discard_count = 3;
        _play_count = 5;
    }
};

class Blind{
    int _score_to_beat;
    int _blind_count;
public:
    Blind(int score): _score_to_beat(score) {}
    int get_score() { return  _score_to_beat; }
    void set_score(int score) {
        _score_to_beat = score;
    }
};

class Ante{
    list<Blind> _blinds;
    double _multiplier_diff = 1;
    static short _global_current;
    short _current = 0;
public:
    Ante(){
        _global_current++;
        for (int i = 0; i < 3; i++) _blinds.emplace_back((300 * _global_current) * _multiplier_diff),
        _multiplier_diff += 0.5;
    }
    list<Blind>& get_blinds() {return  _blinds; }
    short get_current() const { return _current; }
    short get_global_current() const {return  _global_current; }
    Blind& get_current_blind() { return *std::next(_blinds.begin(), _current); }
    void set_current() { _current++;}
};
short Ante::_global_current = 0;

class Shop{
    JokerManager& _joker_manager;
    int& _shop_money;
    vector<unique_ptr<Joker>> _current_offers;
public:
    Shop(JokerManager& jm, int& initial): _joker_manager(jm), _shop_money(initial) {}

    void refresh_offers(){
        _current_offers.clear();
        for(const auto& joker: _joker_manager.get_random_jokers(3)){
            _current_offers.push_back(make_unique<Joker>(*joker));
        }
    }

    void print() const{
        cout << "----Shop (Money: " << _shop_money << ") ----" << endl;
        for(size_t i = 0; i < _current_offers.size(); i++){
            cout << i+1 << ". ";
            _current_offers.at(i)->print();
        }
        cout << "4. Refresh (10)" << endl;
    }

    void buy_joker(int index, vector<unique_ptr<Joker>>& player_jokers){
        if(index < 1 || index > _current_offers.size()) return;
        if(_shop_money >=_current_offers[index-1]->get_price()){
            _shop_money -= _current_offers[index-1]->get_price();
            player_jokers.push_back(make_unique<Joker>(*_current_offers[index-1]));
            _current_offers.erase(_current_offers.begin() + index-1);
        }
    }

    string get_description(int index){
        if(index < 1 || index > _current_offers.size()) return "None";
        return _current_offers[index-1]->get_description();
    }

    void refresh_for_money(){
        if(_shop_money >= 10){
            _shop_money -= 10;
            refresh_offers();
        }
    }
};

class Round{
    Deck _full_deck;
    //Blind _level;
    unique_ptr<Ante> _ante;
    size_t _score = 0;
    short _rounds = 0;
    int _money = 4;
    Shop _shop;
    JokerManager _jm;
    vector<unique_ptr<Joker>> _jokers;
public:
    Round(int score) : _full_deck(), /*_level(score)*/ _ante(make_unique<Ante>()), _score(0),_jm(), _shop(_jm,_money), _jokers(){}
    Deck& get_full_deck() {return _full_deck; }

    void print(){
        cout << "Round: " << _rounds << endl;
        cout << "Money: " << _money << endl;
        cout << "Score to beat: " << _ante->get_current_blind().get_score() << endl;
        cout << "Current score: " << _score << endl;
        cout << "Hands: " << _full_deck.get_play_count() << endl;
        cout << "Discards: " << _full_deck.get_discard_count() << endl;
        cout << "Jokers: " << endl;
        for(const auto& joker: _jokers){
            joker->print();
        }
        cout << endl;
        get_full_deck().get_hand().print();
    }

    void play_hand(){
        _score += _full_deck.play(_jokers);
        if(_score >= _ante->get_current_blind().get_score()){
            _ante->set_current();
            _full_deck.refill_deck();
            cout << "УСПЕХ: " << _score << endl;
            _score = 0;
            _rounds++;
            size_t choice;
            size_t inner_choice;
            _shop.refresh_offers();
            bool now_shop = true;
            while(now_shop){
                _shop.print();
                cout << "Shop menu:\n1. Purchase\n2. Check description\n3. Leave shop\n" << endl;
                cin >> choice;
                switch(choice){
                    case 1:
                        _shop.print();
                        cout << "Purchase for: " << endl;
                        cin >> inner_choice;
                        if(inner_choice > 0 && inner_choice < 4){
                            _shop.buy_joker(inner_choice,_jokers);
                        }
                        else if(inner_choice == 4){
                            _shop.refresh_for_money();
                        }
//                        cout << "Type index: " << endl;
//                        cin >> index;
//                        _shop.buy_joker(index, _jokers);
                        break;
                    case 2:
                        _shop.print();
                        cout << "Read about: " << endl;
                        cin >> inner_choice;
                        if(inner_choice > 0 && inner_choice < 4){
                            cout << _shop.get_description(inner_choice) << endl;
                        }
                        break;
                    case 3:
                        _shop.refresh_offers();
                        now_shop = false;
                        break;
                }
            }
            switch(_ante->get_current()){
                case 1:
                    _money += 3;
                    break;
                case 2:
                    _money += 4;
                    break;
                case 3:
                    _money += 5;
                    _ante = make_unique<Ante>();
                    break;
            }
        }
        if(_full_deck.get_play_count() == 0){
            throw std::out_of_range("ПРОЕБАНО");
        }
    }
};

int main() {
    system("chcp 65001 > null");
//    Deck deck;
    Round round1(300);
    round1.get_full_deck().make_hand(8);
    auto& h = round1.get_full_deck().get_hand();
    h.print();
    size_t choice = 0;
    size_t index = 0;
    while(true){
        cout << "Select command:\n1. Pick card\n2. Cancel picking\n3. Sort by suit\n4. Sort by rank\n5. Discard selected"
                "\n6. Play selected\n" << endl;
        cin >> choice;
        switch(choice){
            case 1:
                round1.print();
                cout << "Select index of card: ";
                cin >> index;
                round1.get_full_deck().get_hand().select(index);
                break;
            case 2:
                round1.print();
                cout << "Select index of card: ";
                cin >> index;
                h.unselect(index);
                break;
            case 3:
                h.sort_by_suit();
                round1.print();
                break;
            case 4:
                h.sort_by_rank();
                round1.print();
                break;
            case 5:
                round1.get_full_deck().discard();
                h.print();
                break;
            case 6:
                round1.play_hand();
                break;
            default:
                return 0;
        }
    }
    return 0;
}
