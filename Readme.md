﻿						ΑΝΑΠΤΥΞΗ ΛΟΓΙΣΜΙΚΟΥ ΜΕΡΟΣ 3

Ονομα			ΕΠΙΘΕΤΟ			Α.Μ.

ΗΛΙΑΣ			ΒΕΡΓΟΣ			1115201400266
ΠΑΝΑΓΙΩΤΗΣ		ΒΛΑΣΣΗΣ			1115201400022
ΔΗΜΗΤΡΙΟΣ		ΛΑΖΑΡΙΔΗΣ		1115201400086



Προγραμματιστικο Περιβαλλον :LINUX 
Eντολη Μεταγλωτισσης: make
Στο Makefile κανουμε 2 μεταγλωτισσεις,1 για το unit test και μια για το κανονικο προγραμμα.
./unitest	-->Unit Test
./myexe		-->main programm

Επεξεργαστής : i5-6500 , 4 πυρήνες , 4 threads , 3.2 GHz
Μνήμη RAM : 16 Gb


							ΓΕΝΙΚΗ ΠΕΡΙΓΡΑΦΗ
Το πρόγραμμα στην αρχή τροφοδοτείται με το σύνολο των σχέσεων (πινάκων) που στέλνονται από το πρόγραμμα harness. Το διάβασμα των ονομάτων των σχέσεων γίνεται με τη συνάρτηση readRelations. Τα αρχεία με τις σχέσεις ανοίγονται μέσω της συνάρτησης createRelations, με την οποία δημιουργούνται οι πίνακες relationArray και statsArray, που περιέχουν τα δεδομένα των σχέσεων και τα στατιστικά για τα δεδομένα αυτά αντίστοιχα. Με την ολοκλήρωση της δημιουργίας των 2 πινάκων, εκτελείται η συνάρτηση executeBatches. Εκεί το πρόγραμμα, λαμβάνει επερωτήσεις που στέλνονται με τη μορφή batches. Ανά batch, γίνεται σειριακή εκτέλεση όλων των επερωτήσεων του. Κάθε επερώτηση (query) αναλύεται μέσω της συνάρτησης analyzeQuery σε 1)Σχέσεις 2)Κατηγορήματα -α)Φίλτρα και β)Joins- 3)Προβολές(Views), δεδομένα που αποθηκεύονται σε αντίστοιχη δομή, που χρησιμοποιείται στη συνέχεια από την executeBatches. Πρώτα εκτελούνται τα κατηγορήματα φίλτρου και μετά τα κατηγορήματα join. Για την σειρά εκτέλεσης των κατηγορημάτων join, χρησιμοποιείται ο αλγόριθμος JoinEnumeration ώστε να υπάρχουν όσο το δυνατόν ελάχιστα ενδιάμεσα αποτελέσματα κατά την εκτέλεση του επερωτήματος (query). Μετά από την εκτέλεση της RadixHashJoin για κάθε κατηγόρημα (predicate) τύπου Join, τα αποτελέσματα μπαίνουν στο middleArray και αν θέλουμε να χρησιμοποιήσουμε σε επόμενο κατηγόρημα ξανά ένα relation που βρίσκεται εκεί,το παίρνουμε από εκεί. Όταν εισάγουμε κάτι στο middleArray, το τοποθετούμε σε μία ομάδα (team). Σε περίπτωση filter ή oneJoin αν δεν υπάρχει στο middleArray, το τοποθετούμε σε νέα ομάδα, ενώ στη περίπτωση που υπάρχει ήδη παραμένει η ίδια ομάδα και αλλάζουμε όλα τα relations που ανήκουν στην ομάδα του. Αφού εκτελεστούν όλα τα κατηγορήματα, γίνεται γράψιμο των αποτελεσμάτων στο STDOUT_FILNO. Η διαδικασία αυτή συνεχίζεται μέχρις ότου να τελειώσει η αποστολή batches από το πρόγραμμα harness (όταν διαβαστούν 0 bytes).
							ΔΟΜΗ ΤΗΣ ΑΣΚΗΣΗΣ
Η άσκηση αποτελείται απο 6 αρχεία.Τα 5 από αυτά είναι για πρόγραμμα μας και το άλλο είναι για το unit testing.Το πρόγραμμα ξεκινά απο την main.c ως πρώτο επίπεδο και στην συνέχεια ως δεύτερο επίπεδο είναι τα αρχεία functions.c/functions.h που αναφέρονται στις βασικές συναρτήσεις που χρησιμοποιεί το πρόγραμμα μας και τα αρχεία Job_S0cheduler.c/Job_Scheduler.h που αναφέρονται στις συναρτήσεις που χρησιμοποιούνται για την υλοποίηση του Job Scheduler.Eπίσης έχουμε οργανώσει το φάκελο μας σε /src που περιέχει όλα τα .c αρχεία και σε /header-files όπου περιέχει τα .h αρχεία και σε /input-files που περιέχει το small dataset μόνο , μιάς και το μεγάλο δεν μπορεί να ανέβει λόγο υπερβολικού μεγέθους.

				ΤΕΛΙΚΗ ΑΝΑΦΟΡΑ ΕΡΓΑΣΙΑΣ ΣΤΗΝ ΑΝΑΠΤΥΞΗ ΛΟΓΙΣΜΙΚΟΥ ΓΙΑ ΠΛΗΡΟΦΟΡΙΑΚΑ ΣΥΣΤΗΜΑΤΑ
Το πρώτο κομμάτι της άσκησης αφορά την δημιουργία της συνάρτησης RadixHashJoin, η οποία είναι και η βασική συνάρτηση του πρώτου μέρους.Η συνάρτηση αυτή δέχεται 2 στήλες , για κάθε στήλη φτιάχνει ένα ιστόγραμμα και στη συνέχεια ένα αθροιστικό ιστόγραμμα με σκοπό να δημιουργήσουμε τους reorder πίνακες. Στη συνέχεια χρησιμοποιώντας τον  reorder πίνακα με το μικρότερο μέγεθος φτίαχνουμε το bucket πίνακα. Χρησιμοποιώντας μία hash function περνάμε κάθε στοιχείο στην κατάλληλη θέση του bucket και υστερα στην κατάλληλη θέση του chain.Μετά συγκρίνουμε τα bucket των δύο reorder πινάκων και αν έχουμε κάποιο ταίριασμα το προσθέτουμε στην λίστα των αποτελεσμάτων. Παρακάτω υπαρχεί εκτενής ανάλυση των συναρτήσεων :
1) hist* createHistArray(oneColumnRelation **rel,int start,int end);
	>Γίνεται η δημιουργία ενός ιστογράμματος (hist) για ένα μέρος μίας σχέση (oneColumnRelation), που προσδιορίζεται από το start
    και το end. Με βάση το υπόλοιπο της διαίρεσης της κάθε τιμής με το μέγεθος του ιστογράμματος (2^n, όπου n ο αριθμός των λιγότερων σημαντικών ψηφίων κάθε αριθμού), 
	προσθέτουμε το σύνολο των εμφανίσεων της τιμής στην αντίστοιχη θέση (αντίστοιχο bucket) του ιστογράμματος (hist).
2) hist* createSumHistArray(hist *array);
	>Χρησιμοποιώντας ένα ιστόγραμμα (array), στη συνάρτηση αυτή γίνεται η δημιουργία
 	του	αθροιστικού ιστογράμματος (hist). Με τη δημιουργία αυτού του αθροιστικού ιστογράμματος, μπορούμε να γνωρίσουμε πόσα values περιέχονται σε κάθε bucket, ώστε
    έτσι να είναι γνωστή η αρχή και το τέλος κάθε bucket. Χρήση αυτού του αθροιστικού ιστογράμματος γίνεται από τη συνάρτηση αναδιάταξης 
    της σχέσης (createReOrderedRelation).
3) oneColumnRelation* createReOrderedArray(oneColumnRelation *array,hist *sumArray,int start,int end,oneColumnRelation *reOrderedArray);
	>Αυτή η συνάρτηση φτιάχνει τον ReOrderedArray (δλδ τον R') χρησιμοποιώντας ένα μέρος (προσδιορίζεται από το start και end) από τον αρχικό πινακα R και τον Rsum,
    ο οποίος για κάθε bucket έχει ενα στοιχείο point. Διαβάζουμε τα στοιχεία από τον αρχικό πίνακα και βλέπουμε σε ποιο bucket ανήκουν, τα βάζουμε στην θέση point του
    ReOrderedArray. Tέλος προσθέτουμε στο point +1, ώστε να δείχνει πλέον στην επόμενη θέση.
4) indexHT* createHashTable(relation* reOrderedArray,int32_t start,int32_t end);
	>Σε αυτήν την συναρτηση περνάμε τον reOrderedArray που έχει το μικρότερο μέγεθος bucket και επίσης περνάμε την αρχή και το τέλος του bucket αυτού έτσι ώστε να  ξέρουμε σε ποια θέση του reOrderedArray θα πάμε και θα παρουμε τα στοιχεια.Στην συνέχει πέρνοντας 1-1 τα στοιχέια τα περναμε απο μια hashFunction που βρήκαμε ετοιμη στο διαδικτυο (https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key), και τα τοποθετουμε μεσα στο καταλληλο bucket και στην συνεχεια στην καταλληλη θεση του chain. 
5) void compareRelations(indexHT *ht,relation *array,int32_t start,int32_t end,relation *hashedArray,resultList *resList,int32_t );
	>Σε αυτή την συνάρτηση περνάμε ενα ενα τα στοιχεία του bucket με τα πολλά στοιχεία από την hash (έχουμε κάνει ηδη ένα hash table sto bucket με τα λίγα στοιχεία),
	πηγαίνουμε στην θέση του bucket που μας λέει το hash και παίρνουμε το offset για την chain , αν είναι -1 τότε δεν υπάρχει τέτοιο στοιχείο , σε αντίθετη περίπτωση
	τρέχουμε τον πίνακα chain πηγαίνοντας κάθε φορά στο prevchainPosition , τα αποτελέσματα τα βάζουμε με την insertResult στο resultList.
6) void insertResult(resultList *list,uint32_t id1,uint32_t id2,int32_t);
	>Σε αυτην την συναρτηση πέρνουμε τα 2 id που εχουμε συγκρίνει και εχουμε δει οτι τα values τους ειναι ιδια και στην συνέχεια τα τοποθετουμε στην καταλληλη θεση μεσα στην λιστα.Επίσης εχουμε και ενα ορισμα παραπανω στην συναρτηση αυτη ,το fromArray ,και αυτο το κανουμε για να ξερουμε καθε φορα ποιο bucket καναμε hash και ποιο οχι.Ετσι τοποθετουμε τα ids στους σωστους πινακες και η λιστα μας εχει πλέον φτιαχτει.Καθε κομβος της λιστας θα εχει 128*1024= 131.072 / (32bits + 32bits => 4 + 4 = 8bytes) = 16.384 αποτελεσματα.
	
Σχολιο : οι παραπανω συναρτησεις ειναι διαφοροποιημενες απο αυτες του πρωτου part οσων αφορα τις παραμετρους και καποιες αλλαγες στο σωμα των συναρτησεων,γιατι αναφερονται σε υλοποιηση με threads.
Στο πρώτο κομμάτι δεν είχαμε κάποιο dataset γιαυτό και δεν έχουμε μετρήσεις και unit testing κάναμε δικές μας δοκιμές χωρίς να χρησιμοποιούμε κάποιο framework.


Το δευτερο κομματι ξεκινήσαμε με την σύνδεση του harness με το δικό μας πρόγραμμα ώστε να μπορούμε να ελέγχουμε την ορθότητα και τον χρόνο εκτέλεσης του προγράμματός μας.Γιαυτό φτιάξαμε κάποιες καινούργιες συναρτήσεις οι οποίες παραθέτονται παρακάτω:

1) char **readRelations(int *number_of_files);
    > Διαβάζονται τα ονόματα των relations που στέλνονται από το harness και επιστρέφεται ένας πίνακας με τα ονόματα αυτών των relations.
2) void createRelations(int number_of_files,multiColumnRelation **relationArray,all_stats **statsArray,char **relation);
    > Με βάση τον πίνακα με τα ονόματα των relations, γίνεται διάβασμα των relations αυτών και δημιουργούμε τον πίνακα relationArray, όπου αποθηκεύονται τα δεδομένα τους και τον πίνακα statsArray, όπου κρατάμε στατιστικά για τα δεδομένα αυτά.
3) void executeBatches(multiColumnRelation *relationArray,all_stats *statsArray);
    > Η συνάρτηση διαβάζει batches και τα εκτελεί, τα οποία batches στέλνονται μέσω του harness. Μόλις διαβάσει 0 bytes, τερματίζει.

Σχόλιο : Οι παραπάνω συναρτήσεις καλούνται στην main και βοηθούν στην ομαλή ροή εκτέλεσης του προγράμματος
Στην συνέχεια ασχοληθήκαμε με την κατασκευή του ενδιάμεσου πίνακα αποτελεσμάτων, ο οποίος αποτελείται από έναν πίνακα από στήλες που αντιστοιχούν είτε σε κάποια στήλη του αρχικού πίνακα είτε σε κάποιο αποτέλεσμα ενός φίλτου , μίας ζεύξης 2 στηλών απο διαφορετικό πίνακα ή απο ίδιο πίνακα. Κάθε τέτοια στήλη περιλαμβάνει ένα id μιας σχέσης στην οποία ανήκει και έναν αριθμό "ομάδας" που μας δείχνει με ποιές άλλες στήλες της ίδιας ομάδες έχει συζευχθεί.Τα αποτελέσματα από τα φίλτρα μπαίνουν πάντα πρώτα στον ενδιάμεσο πίνακα αποτελεσμάτων γιατί εκτελούνται πρώτα και σε περίπτωση εισαγωγής στήλης πάνω σε ήδη υπάρχουσα στηλη γίνεται αντικατάσταση.Όσων αφορά τα αποτελέσματα της ζευξης 2 στηλών , εισάγονται κανονικά σε περίπτωση που η αντίστοιχη στήλη δεν υπάρχει στον ενδιάμεσο πίνακα αλλίως αν τα αποτελέσματα της καινούργιας στήλης είναι περισσότερα κάνουμε realloc στην στήλη, αν είναι λιγότερα απλά μειώνουμε την μεταβλητή που περιέχει τον αριθμό των αποτελεσμάτων.Αν κάποια στήλη αλλάξει κατά την εισαγώγή καινούργιων αποτελεσμάτων τότε θα πρέπει να αλλάξουν και οι στήλες που ανήκουν στην ίδια ομάδα με αυτήν.Η εισαγωγή των καινούργιων αποτελεσμάτων γίνεται μέσω ενός βοηθητικού πίνακα μεγέθους ίσο με τον αριθμό των αποτελεσμάτων , και αυτό γίνεται για να μην χάσουμε την αντιστοίχηση των στηλών του ενδιάμεσου πίνακα με τις στήλες του αρχικού.Αυτό βέβαια έχει ως αποτέλεσμα μεγάλη σπατάλη μνήμης γιατί δημιουργούμε στην ουσία έναν πίνακα όσος και ο αριθμός των αποτελεσμάτων πράγμα που είναι εκτός απο χρονοβόρο και μεγάλη σπατάλη πόρων. Σκεφτήκαμε διάφορους τρόπους για να γλυτώσουμε αυτήν την υλοποίηση.Όσοι από αυτούς υλοποιήθηκαν δεν απέδοσαν χρονικά και οι υπόλοιποι ήταν πολύ δύσκολο να υλοποιηθούν  και γιαυτό δεν σπαταλήσαμε παραπάνω χρόνο.
Επιπλέον προσπαθήσαμε να δημιουργήσουμε τα δικά μας στατιστικά , κρατάγαμε το μέγιστο , το ελάχιστο , τον μέσο όρο και τις διακριτές τιμές.Για τις διακριτές τιμές τις υπολογίζαμε με την βοήθεια ενός hash table αλλά και πάλι η ευρεσή τους ήταν πολύ αργή. Για τον λόγο αυτό δοκιμάσαμε και μία πιθανοτική προσέγγιση εύρεσης διακριτών τιμών ο οποίος ήταν σαφώς πιο γρήγορος αλλα όχι τόσο αποτελεσματικός, και επειδή μας ενδιαφέρει ο χρόνος διαλέξαμε αυτόν.Παρακάτω παρουσιάζονται οι χρόνοι εκτέλεσης του δεύτερου μέρους :

|	| small         | public        |
| ----- | ------------- |:-------------:|
|Time	| 2.6s          | 2m8s          |

Σχόλιο : Το harness υλοποιήθηκε στο τρίτο μέρος, απλως αναφέρουμε ότι έγινε στην αρχή του δεύτερου μερους γιατί εκέι έπρεπε να γίνει κανονικά.
Όσων αφορά το τρίτο μέρος τα μέρη που μας απασχόλησαν κυρίως ήταν η δημιουργία στατιστικών για την εύρεση της βέλτιστης σειρά εκτέλεσης των join καθώς και η δημιουργία και εκτέλεση των νημάτων.Παρακάτω βρίσκεται η επεξήγησή τους.

							Threads και Job_Scheduler
1) Job_Scheduler* initialize_scheduler(int execution_threads,oneColumnRelation *R,oneColumnRelation *S)
    >Αρχικοποίηση του job Job_Scheduler,δημιουργούμε την ουρά ,τα threads και τον Job_Scheduler.Περνάμε σαν ορίσματα των αριθμό των threads που θα δημιουργηθούν και τις δύο στήλες στις οποίες θα πραγματοποιηθεί η ζεύξη.Ακόμα, μέσα σε αυτήν την συνάρτηση δημιουργούμε και μια μεταβλητή,που θα περνάμε σαν όρισμα σε κάθε thread_worker ,και περιλαμβάνει τον Job_Scheduler που φτιάξαμε νωρίτερα ,ένα δικό μας id για κάθε thread,το R ,και το S.Τέλος στον Job_Scheduler έχουμε δημιουργήσει μια δομή shared_data η οποία περιλαμβάνει κάποιες μεταβλητές που θα διαμοιράζονται η κεντρική διεργασία (RadixHashJoin) και τα threads (Workers).Mέσα σε αυτήν έχουμε προσθέσει 2 πίνακες με hist (1 για κάθε σχέση R,S),2 πίνακες tuples (1 για κάθε σχέση R,S),και ένα πίνακα με λίστες για τα join jobs.
2) void submit_Job(Job_Scheduler* schedule,Job Job)
    >Μέσω αυτής της συνάρτησης υπόβαλουμε ένα job στον Job_Scheduler και αυξάνουμε τον μετρητή των δουλειών.
3) Job initializeJob(char *type_of_job)
    >Ανάλογα με το είδος δουλείας εχουμε να κάνουμε (hist,partition,join) κάνουμε και την κατάλληλη αρχικοποίηση της δουλείας και ύστερα την επιστέφουμε.
4) void delete_threads(Job_Scheduler** schedule)
    >Η συνάρτηση αυτή περιμένει τα threads να τερματίσουν την λειτουργία τους και ύστερα καταστρέφει τον mutex της ουρας και απελευθερώνει τον χώρο που είχε πιάσει ο Job_Scheduler.
5) void sleep_producer(Job_Scheduler *job_scheduler,int end)
    >H συνάρτηση αυτή καλείται κάθε φορά απο την RadixHashJoin ,σύνολο 3 φορές , μια για κάθε job (hist,partition,join),και blockarei την κεντρική διεργασία σε ένα condition variable εώς ότου όλες οι δουλειές που έχουν ανατεθεί στον Job_Scheduler να τελείωσουν.Μόλις ο μετρητής των δουλειών δείξει μηδεν τότε η κεντρική διεργασία ξεμπλοκάρει και συνεχίζει την λειτουργία της RadixHashJoin.Όταν η διεργασία λαβει στο όρισμα end την τιμη 1 σημαίνει οτι τελείωσαν και οι δουλειές join και πρέπει τα threads να τερματίσουν την λειτουργία τους.Αυτο γίνεται μέσω της global μεταβλητής ending που παίρνει την τιμή 1 και επιτρέπει στα threads ,που μέχρι τωρα είχαν blockarei απο condition variable, να τερματισουν.
6) void *Worker(void* j)
    >Η συνάρτηση αυτή είναι η συνάρτηση που τρέχουν τα threads κατα την λειτουργία τους.Πέρνουν σαν όρισμα μία μεταβλητή που την εξήγησαμε παραπάνω στο 1).Τα threads ξεκινούν πέρνοντας μία θεση στην ουρά.Επομένως έχουμε ένα mutex ίσο με τον αριθμό των θέσεων της ουράς.Στην συνέχεια, αν η θεση αυτή είναι άδεια τότε το thread blockarei στον condition variable με όνομα worker_wait_cond μέχρι η κεντρική διεργασία το ξυπνήσει και του αναθέσει κάποια δουλειά.Αν πάλι η θέση που καταλαμβάνει είναι γεμάτη(έχει μέσα δουλεία) τότε το thread δεν μπλοκάρει στον condition variable ,ξεκλειδώνει το mutex που είχε πάρει και στην συνέχεια εκτελεί την δουλεία που πήρε.Η δουλεία όπως είπαμε και παραπάνω στο 2) χωρίζεται στα 3(hist,partition,join).Mόλις το thread τελειώσει με την δουλεία που του ανατέθηκε μπαίνει σε ένα mutex για να μειώσει τον αριθμο των δουλειών του Job_Scheduler.Tέλος μειώνοντας το μετρητή των δουλειών στέλνει ενα σήμα στο condition variable που έχει μπλοκάρει η κεντρική διεργασία για να ελένξει αν ο μετρητή των δουλειών έχει φτάσει στο 0,και μετά επαναλαμβάνεται όλη η παραπάνω διαδικασία μέχρι τα threads να λαβουν το ending=1 που σηματοδοτεί τον τερματισμό τους.




int *JoinEnumeration(queryDataIndex *data,all_stats *before_joins_stats)
	>Η συνάρτηση JoinEnumeration επιστρέφει έναν πίνακα ο οποίος μας λέει με ποια σειρά θα γίνουν τα Join , εκτελούμε πρώτα όλα τα Filter οπότε τα στατιστικά που μας έρχονται δεν είναι από όταν διαβάσαμε στην αρχή τα relations , αλλά άλλαξαν στην συνάρτηση executeFilter. Αρχικά η συνάρτηση φτιάχνει έναν γράφο που περιέχει όλες τις σχέσεις μεταξύ των πινάκων που συνδέονται με κατηγορήμα. Το bestTree είναι ένας πίνακας με λίστες που περιέχουν όλους τους συνδιασμούς με κ πίνακες σετ μεγεθους κ , για το επόμενο "κουτί" του πίνακα παίρνουμε το καλύτερο από το προηγούμενό του και ελέγχουμε με ποια φτιάχνει κατηγόρημα.Αρχικά στο πρώτο for βάζω τους πίνακες τον καθένα μόνο του , δηλαδή φτιάχνω σετ μεγέθους 1. Ο κάθε κόμβος περιέχει ποιοι πίνακες είναι στο σετ , ποιες είναι οι ομάδες στα middle αποτελεσματα , ποια είναι η σειρα και τα στατιστικά του.Οπότε φτιάχνω ένα αντιγραφο των στατιστικών για να τα βάλω στον κόμβο.Πριν τα βάλω ελέγχω αν έχουμε Join με στηλες του ίδιου πίνακα ,αν ναι τότε υπολογίζω τα νέα στατιστικά μετα το Join και τα βάζω στην λίστα,αλλίως απλά τα προσθέτω στην λίστα.Το δεύτερο for της εκφώνησης το έχω σπάσει σε 2 κομμάτια , το πρώτο κομμάτι είναι παρόμοιο με το πρώτο for δηλαδή τρέχω όλη την λίστα του btree[0] και βρίσκω όλα τα ζευγάρια (σετ μεγέθους 2) που μπορώ να φτιάξω χρησιμοποιώντας τον γράφο , αλλάζω τα στατιστικά ανάλογα με τα κατηγορήματα που μπορούν να εκτελεστούν και τα προσθέτω στο btree[1]. Το δεύτερο κομμάτι δεν παίρνει όλους τους κόμβους αλλά μόνο τον καλύτερο από το προηγούμενη θέση του πίνακα και πάλι με παρόμοιο τρόπο ελέγχει για νέα σετ ,φτιάχνει ένα αντίγραφο των στατιστικών υπολογίζει τα νέα στατιστικά και τα τοποθετεί στην λίστα.Σε κάθε λίστα υπάρχει ένας πίνακας seira ο οποίος περιέχει για κάθε υποσετ το καλύτερο τρόπο να εκτελεστούν τα κατηγορήματα με βαση τους πίνακες που έχει, στην τελευταία θέση του πίνακα υπάρχει μόνο ένας κόμβος που περιέχει την σειρά που πρέπει να κάνουμε return.Η επιλογή του BestNode γίνεται από το cost , επιλέγουμε τον κόμβο με το μικρότερο κόστος, όπου το κόστος είναι πόσα ενδιάμεσα αποτελέσματα θα έχουμε μετά την εκτέσεση μιας υποομάδας κατηγορημάτων. 
	>Ο υπολογισμός των στατιστικών γίνεται απο τις εξής συναρτήσεις:
		>greaterThanFilterStatsCalculator
		>lessThanFilterStatsCalculator
		>sameJoinStatsCalculator
		>RadixStatsCalculator
		>Για τις οποίες εκτελούμε τους τύπους που μας δώθηκαν στην εκφώνηση,στην RadixStatsCalculator χρησιμοποούμε της συναρτήσεις greaterThanFilterStatsCalculator και lessThanFilterStatsCalculator ώστε να φτιάξουμε τα min kai max στις στήλες που κάνουμε Radix.



Εδώ παρουσιάζονται οι χρόνοι εκτέλεσης με δοκιμές για δίάφορες τιμές των threads,του Ν και του bucketPosNum:

Για τα αρχεία “small.*”



| N             | Αριθμός θέσεων του bucket (bucketPosNum)   | Time  |
|:-------------:|:------------------------------------------:|:-----:|
|        3      |                                      10    | 2.33s |
|        3      |                                      30    | 1.20s |
|        3      |                                      80    | 0.85s |
|        5      |                                      10    | 0.95s |
|        5      |                                      30    | 0.78s |
|        5      |                                      80    | 0.72s |
|       10      |                                      10    | 0.75s |
|       10      |                                      30    | 0.69s |
|       10      |                                      80    | 0.90s |
|       15      |                                      10    | 0.95s |
|       15      |                                      30    | 0.91s |
|       15      |                                      80    | 0.93s |
|       20      |                                      10    | 4.10s |
|       20      |                                      30    | 4.50s |
|       20      |                                      80    | 4.30s |
