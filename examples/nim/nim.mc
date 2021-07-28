/*
  The computers realy stupid strategy
  It is only able to win if the human lets the computer make the first move
*/
int computer_strategy(int pile, int human_last_choice)
{
    int choice;

    if(human_last_choice == 0)
    {
        {
            { return 2; }
        }
    }

    choice = 2 * 2 - human_last_choice;

    if( (pile - choice ) < 0 )
    {
        return 1;
    }

    return choice;
}

void print_pile_size(int pile)
{
    print("Size of the match stick pile: ");
    print_int(pile);
    print_nl();
}

void print_removal(string player, int choice)
{
    print(player);
    print(" removed: ");
    print_int(choice);
    print_nl();
}

void game(int pile, int start)
{
    int human_choice;
    int computer_choice;
    int next;

    string opponent;

    string computer_chose;
    computer_chose = "Computer removed: ";

    string human_chose;
    human_chose = "You decided to remove: ";

    if( start == 1 )
    {
        opponent = "Human";

        computer_choice = computer_strategy(pile, 0);
        pile = pile - computer_choice;

        print_removal("Computer", computer_choice);
    }

    while( pile > 0 )
    {
        opponent = "Computer";

        print_pile_size(pile);

        print("How many match sticks do you want to take away? Between 1 and 3.");
        print_nl();

        human_choice = read_int();
        next = pile - human_choice;

        if( (human_choice <= 3) && ( next >=0 ) )
        {
            pile = next;

            print_removal("You", human_choice);
            print_pile_size(pile);

        }
        else
        {
            print("Invalid move. You must take between 1 and 3 match sticks. But you tried to take ");
            print_int(human_choice);
            print(" match sticks");
            print_nl();
        }

        if( human_choice <= 3 && pile>0)
        {

            opponent = "Human";

            computer_choice = computer_strategy(pile, human_choice);
            pile = pile - computer_choice;

            print(computer_chose);
            print_int(computer_choice);
            print_nl();
        }
    }

    print(opponent);
    print(" wins.");
    print_nl();
}

int main()
{
    int pile;
    int start;

    pile = 15;

    print("NIM: The match stick game");
    print_nl();
    print(".........................");
    print_nl();

    print("Size of the match stick pile: ");
    print_int(pile);
    print_nl();

    print("Who should start the game? 0 for You 1 for Computer");
    print_nl();
    start = read_int();

    if( start < 0 || start > 1 )
    {
        print("FAILURE: Start was ");
        print_int(start);
        print("but should have been between 0 and 1");
        print_nl();
        return 1;
    }

    game(pile, start);

    return 0;
}
