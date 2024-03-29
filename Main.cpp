#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

using namespace sf;

// Data definitions:
/**
 * A CardValue is an int in the range [1, 13] where:
 * - 1 represents an Ace
 * - [2, 10] represents the number card
 * - 11 represents a Jack
 * - 12 represents a Oueen
 * - 13 represents a King
 * 
 * @relatesalso Suit
 * @relatesalso Card
 */
#define CardValue int
/**
 * A Suit is an int in the range [0, 3] where:
 * - 0 represents Clubs
 * - 1 represents Diamonds
 * - 2 represents Spades
 * - 3 represents Hearts
 * 
 * @relatesalso CardValue
 * @relatesalso Card
 */
#define Suit int
/**
 * A Card is a std::pair<CardValue,Suit>
 * 
 * A makeCard(first, second) where:
 * - first represents the value of the card
 * - second represents the suit of the card
 * 
 * @relatesalso CardValue
 * @relatesalso Suit
 */
#define Card std::pair<CardValue,Suit>
#define makeCard std::make_pair
/**
 * A Hand is a std::vector<Card >
 * 
 * A hand represents a collection of cards.
 * 
 * @relatesalso Card
 */
#define Hand std::vector<Card >

// Constants:
/**
 * Number of standard 52-card decks to be played with.
 *
 * @hideinitializer
 */
const int deckCount = 1;
/**
 * Size of card assets.
 *
 * @hideinitializer
 */
const int cardWidth = 140, cardHeight = 190;
/**
 * Position of the center of a card relative to TOP LEFT.
 *
 * @hideinitializer
 */
const Vector2f middleCard(cardWidth/2, cardHeight/2);
/**
 * Number of pixels to offset stacked cards so bottom card is still readable.
 *
 * @hideinitializer
 */
const float cardStackOff = cardWidth/3.5f;
/**
 * Card representing the absence of a card.
 *
 * @hideinitializer
 */
const Card NULL_CARD = makeCard(-1, -1);

// Header for functions defined below main:
void shuffleDeck(Hand* deck);
Card drawCard(Hand* deck, int* i, const int deckSize);
int maxHandValueNoBust(Hand* hand);
void renderCard(RenderWindow* window, Texture* t, Card* card, Vector2f pos, bool rotate);

int main() {
    VideoMode vm(1280, 720);
    RenderWindow window(vm, "King of Blackjack");

    Texture cardBackTexture;
    if(!cardBackTexture.loadFromFile("./assets/images/cardBack_red3.png"))
        std::cout << "Could not load file 'cardBack_red3.png' at directory './assets/images/'" << std::endl;
    Sprite cardBack;
    cardBack.setTexture(cardBackTexture);
    cardBack.setOrigin(middleCard);
    Texture emptyDeckTexture;
    if(!emptyDeckTexture.loadFromFile("./assets/images/cardBack_green0.png"))
        std::cout << "Could not load file 'cardBack_green0.png' at directory './assets/images/'" << std::endl;

    Texture cardSpritesheet;
    if(!cardSpritesheet.loadFromFile("./assets/images/cardSpritesheet.png"))
        std::cout << "Could not load file 'cardSpritesheet.png' at directory './assets/images/'" << std::endl;

    Hand deck;
    for(int i = 0; i < deckCount * 52; i++) {
        const int indexInDeck = i % 52;
        const int value = indexInDeck % 13 + 1;
        const int suit = indexInDeck / 13;
        Card pair = makeCard(value, suit);
        deck.push_back(pair);
    }
    shuffleDeck(&deck);

    const int DECK_SIZE = deck.size();
    int nextCardIndex = 0;

    Hand dealerHand;
    bool dealerFlipped = false;
    dealerHand.push_back(drawCard(&deck, &nextCardIndex, DECK_SIZE));
    dealerHand.push_back(drawCard(&deck, &nextCardIndex, DECK_SIZE));
    //std::cout << maxHandValueNoBust(&dealerHand) << std::endl;

    Hand playerHand0, playerHand1;
    bool playerHand0Double = true, playerHand1Double = false;
    std::pair<Hand*,Hand*> playerHand = makeCard(&playerHand0, &playerHand1);

    playerHand0.push_back(drawCard(&deck, &nextCardIndex, DECK_SIZE));
    playerHand0.push_back(drawCard(&deck, &nextCardIndex, DECK_SIZE));
    playerHand0.push_back(drawCard(&deck, &nextCardIndex, DECK_SIZE));
    //std::cout << maxHandValueNoBust(playerHand.first) << std::endl;

    playerHand1.push_back(drawCard(&deck, &nextCardIndex, DECK_SIZE));
    playerHand1.push_back(drawCard(&deck, &nextCardIndex, DECK_SIZE));
    playerHand1.push_back(drawCard(&deck, &nextCardIndex, DECK_SIZE));
    //std::cout << maxHandValueNoBust(playerHand.second) << std::endl;

    while(window.isOpen()) {

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if(Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }
        
        window.clear(sf::Color(99, 149, 81));

        const int dealerHandSize = dealerHand.size();
        for(int i = 0; i < dealerHandSize; i++) {
            Card card = dealerHand.at(i);
            float xoff = (i-(dealerHandSize-1)/2.0f)*cardWidth;
            Vector2f pos(float(vm.width)/2.0f + xoff*9/8, cardHeight*3/4);
            if(i > 0 || dealerFlipped) {
                renderCard(&window, &cardSpritesheet, &card, pos, false);
            } else {
                cardBack.setPosition(pos);
                window.draw(cardBack);
            }
        }

        const bool twoHands = playerHand1.size() > 0;
        const int playerHand0Size = playerHand0.size();
        const int playerHand1Size = playerHand1.size();
        for(int i = 0; i < playerHand0Size; i++) {
            Card card = playerHand0.at(i);
            float xoff = (i-(playerHand0Size-1)/2.0f)*cardStackOff;
            const float yoff = i*cardStackOff;
            if(twoHands) xoff -= playerHand0Size*cardStackOff + cardWidth/4.0f;
            Vector2f pos(vm.width/2.0f + xoff, vm.height - (cardHeight*3/4 + yoff));
            if(i == 1 && playerHand0Double) renderCard(&window, &cardSpritesheet, &card, Vector2f(pos.x + cardStackOff/1.7f, pos.y - cardStackOff/2.0f), true);
            else renderCard(&window, &cardSpritesheet, &card, pos, false);
        }
        for(int i = 0; i < playerHand1Size; i++) {
            Card card = playerHand1.at(i);
            float xoff = (i-(playerHand1Size-1)/2.0f)*cardStackOff;
            const float yoff = i*cardStackOff;
            xoff += playerHand1Size*cardStackOff + cardWidth/4.0f;
            Vector2f pos(vm.width/2.0f + xoff, vm.height - (cardHeight*3/4 + yoff));
            if(i == 1 && playerHand1Double) renderCard(&window, &cardSpritesheet, &card, Vector2f(pos.x + cardStackOff/1.7f, pos.y - cardStackOff/2.0f), true);
            else renderCard(&window, &cardSpritesheet, &card, pos, false);
        }

        Sprite deckSprite;
        if(nextCardIndex < DECK_SIZE) {
            deckSprite = cardBack;
        } else {
            deckSprite.setTexture(emptyDeckTexture);
            deckSprite.setOrigin(middleCard);
        }
        deckSprite.setPosition(Vector2f(middleCard.x*1.5f, vm.height/2));
        window.draw(deckSprite);

        window.display();
    }
    
    return 0;
}

/**
 * Randomly shuffle a deck of cards.
 *
 * @param deck : pointer to a vector of Cards.
 */
void shuffleDeck(Hand* deck) {
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle((*deck).begin(), (*deck).end(), g);
}

/**
 * Return the next Card in the deck,
 * if no next card exists, return NULL_CARD.
 *
 * @param deck : pointer to a vector of Cards.
 * @param i : pointer to index of next Card.
 * @param deckSize : total number of cards in the deck.
 * @return : next Card in the deck.
 */
Card drawCard(Hand* deck, int* i, const int deckSize) {
    if((*i) < deckSize) {
        Card c = (*deck).at((*i));
        (*i)++;
        return c;
    }
    return NULL_CARD;
}

/**
 * Return the value of a hand closest to 21 without going over,
 * if no value exists that is at most 21, return the lowest value.
 *
 * @param hand : pointer to a vector of Cards.
 * @return : sum of Card values.
 */
int maxHandValueNoBust(Hand* hand) {
    const int handSize = (*hand).size();
    int value = 0;
    int aces = 0;
    for(int i = 0; i < handSize; i++) {
        int cardValue = (*hand).at(i).first;
        if(cardValue == 1) aces++;
        if(cardValue >= 10) value += 10;
        else value += cardValue;
    }
    while(aces > 0 && value < 12) {
        value += 10;
        aces--;
    }
    return value;
}

/**
 * Render a Card onto the window at pos, rotate card if it should be sideways.
 *
 * @param window : pointer to the sfml RenderWindow.
 * @param cardSpritesheet : pointer to the cardSpritesheet Texture.
 * @param card : pointer to the Card to draw.
 * @param pos : position to place the card in the window.
 * @param rotate : should the card be rotated?
 */
void renderCard(RenderWindow* window, Texture* cardSpritesheet, Card* card, Vector2f pos, bool rotate) {
    Sprite cardSprite((*cardSpritesheet));
    cardSprite.setTextureRect(IntRect(((*card).first-1)*cardWidth, (*card).second*cardHeight, cardWidth, cardHeight));
    cardSprite.setOrigin(middleCard);
    cardSprite.setPosition(pos);
    if(rotate) cardSprite.rotate(90);
    (*window).draw(cardSprite);
}