Testo dell'esercizio, basato su assignment9/capitalizer-mt, riportato di seguito

Realizzare un programma C che implementa un server che rimane sempre attivo in
attesa di richieste da parte di uno o piu' processi client su una socket di tipo
AF_UNIX. Ogni client richiede al server la trasformazione di tutti i caratteri di
una stringa da minuscoli a maiuscoli (es. ciao –> CIAO). Per ogni nuova connessione
il server lancia un thread POSIX che gestisce tutte le richieste del client
(modello “un thread per connessione”) e quindi termina la sua esecuzione quando
il client chiude la connessione.

Per testare il programma, lanciare piu' processi client ognuno dei quali invia una
o piu' richieste al server multithreaded.



Modificare l'Esercizio 3 dell'Esercitazione 9 in modo da gestire la terminazione
del server a seguito della ricezione di un segnale di terminazione
(SIGINT, SIGQUIT, SIGTERM, SIGHUP) con l'obiettivo di lasciare il sistema in uno
stato consistente, cioè eliminare dal file system il socket AF_UNIX creato per
accettare le connessioni, eventuali file temporanei e liberare la memoria allocata
dinamicamente al fine di poter controllare eventuali memory leaks con valgrind.
Il segnale SIGPIPE deve essere invece ignorato.

La gestione dei segnali deve essere effettuata installando un signal-handler con
la SC sigaction. Il signal-handler deve accedere solamente a variabili dichiarate
volatile sig_atomic_t e deve usare solo chiamate asynchronous-signal-safe
(vedere anche man 7 signal-safety). Testare l'esecuzione del server lanciandolo
con il comando valgrind –leak-check=full e verficare che non ci siano messaggi di
errore all'uscita dopo aver inviato un segnale di terminazione al termine del test.



Modificare l'Esercizio 2 in modo da gestire i segnali nel server in modo sincrono
utilizzando un thread dedicato (non devono essere installati signal-handlers – se non
per ignorare SIGPIPE) e la chiamata di libreria sigwait. Fare attenzione a bloccare
tutti i segnali che si vogliono gestire in tutti i threads al fine di evitare il
delivery “accidentale” ad un thread diverso dal thread gestore dei segnali.
