#include <boost/shared_ptr.hpp>
#include "chat.h"

int main()
{
    boost::shared_ptr<Chat> chat(new Chat);

    chat->Autenticacao();
    chat->InciciarChat();

    return EXIT_SUCCESS;
}
