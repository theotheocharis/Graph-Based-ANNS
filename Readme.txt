PROJECT 2023 - ΑΣΚΗΣΗ 2 - Graph-Based-ANNS (GNNS - MRNG)

ΟΜΑΔΑ 32
ΘΕΟΧΑΡΗΣ ΘΕΟΔΩΡΟΣ - 1115201800054
ΚΙΚΙΔΗΣ ΔΗΜΗΤΡΙΟΣ - 1115201600258

GITHUB REPO: https://github.com/theotheo7/Graph-Based-ANNS

ΓΕΝΙΚΑ

Το project υλοποιήθηκε σε C++17 και με τη χρήση του github. Για τη
μεταγλώττιση υπάρχει διαθέσιμο Makefile.

Για τη μεταγλώττιση όλων των εκτελέσιμων:
    make all

Για τη μεταγλώττιση και εκτέλεση του lsh με default παραμέτρους:
    make lsh

Για τη μεταγλώττιση και εκτέλεση του hypercube με default παραμέτρους:
    make cube

Για τη μεταγλώττιση και εκτέλεση του GNNS με default παραμέτρους:
    make graph1

Για τη μεταγλώττιση και εκτέλεση του MRNG με default παραμέτρους:
    make graph2

Για εκτέλεση ανεξάρτητα από το Makefile:
    ./build/<exec> <flags>

Για τον καθαρισμό όλων των εκτελέσιμων και των object files:
    make clean

Οι δύο διαφορετικοί αλγόριθμοι χρησιμοποιούν την ίδια main. Αρχικά κάνει parse
τα command line arguments και αρχικοποιεί τις παραμέτρους με βάση αυτά. Αν
ο χρήστης δεν του δώσει τα ονόματα των αρχείων τα ζητάει explicitly. Ύστερα,
με τη χρήση του parser διαβάζει τα input images και τα query images και μετά,
ανάλογα με την τιμή του m, χρησιμοποιεί μία από τις μεθόδους. Αφού τελειώσει
και γράψει τα αποτελέσματα στο output file ρωτάει τον χρήστη αν θέλει να κάνει
νέο query. Αν όχι, αποδεσμεύει όλη τη μνήμη που είχε δεσμεύσει και τελειώνει.

Τα αρχεία του lsh/hypercube χρησιμοποιήθηκαν αυτούσια από το πρώτο παραδοτέο,
χωρίς βέβαια τις λειτουργίες που εξυπηρετούσαν το clustering και με μερικές
ελάχιστες αλλαγές/επεκτάσεις για την εξυπηρέτηση του τωρινού παραδοτέου. Το
ίδιο ισχύει και για τα αρχεία για το image αλλά και κάποια utility functions.

ΜΕΡΟΣ Α - GNNS 

    gnns.cpp/hpp

        Constructor (GNNS::GNNS): Αρχικοποιεί τον αλγόριθμο GNNS με συγκεκριμένες παραμέτρους.

        Graph Construction (GNNS::constructGraph):Κατασκευάζει τον γράφο k-NN χρησιμοποιώντας Locality Sensitive Hashing (LSH)
                                                    για να βρει k πλησιέστερους γείτονες για κάθε εικόνα του input dataset.

        Search Method (GNNS::search): Εκτελεί τον αλγόριθμο GNNS για το δοσμένο query image. Αρχικοποιείται ένα διάνυσμα candidates
        για την αποθήκευση των υποψήφιων nearest neighbors και των αποστάσεών τους από το query. Χρησιμοποιείται ένα σύνολο neighborsSet για να αποφευχθεί
        η επαναληπτική εξέταση των ίδιων images. Επιπλέον, ορίζονται μεταβλητές για την καταγραφή του χρόνου εκτέλεσης της αναζήτησης για τις approximate και true αποστάσεις.
        Η μέθοδος getTrueNeighbors καλείται για να υπολογίσει τις true distances από την query image προς όλα τα images του dataset.
        Πραγματοποιούνται R τυχαίες επανεκκινήσεις (random restarts). Σε κάθε επανεκκίνηση, επιλέγεται τυχαία μία εικόνα από το σύνολο δεδομένων.
        Για κάθε επιλεγμένη εικόνα, εκτελούνται έως maxGreedySteps αναζητώντας nearest neighbors. Σε κάθε βήμα, ελέγχεται αν η εικόνα που εξετάζεται 
        είναι πιο κοντά στην query image από την προηγούμενη και αν ναι, προστίθεται στο διάνυσμα candidates. 
        Η διαδικασία αυτή συνεχίζεται μέχρι να μη βρεθεί πλησιέστερη εικόνα ή να φτάσουμε το μέγιστο αριθμό βημάτων.

        Output Time and MAF (GNNS::outputTimeMAF): Εξάγει τους average times για approximate και true distance υπολογισμούς
                                                 καθώς και το MAF για όλα τα queries. 

        Get True Neighbors (GNNS::getTrueNeighbors):Υπολογίζει τις true distances μεταξύ ενός query image και όλων των images του dataset.        
    
        Output Results (GNNS::outputResults): Εξάγει τα search results για ενα δοσμένο query image συμπεριλαμβάνοντας τo ID του query
                                              και τις αποστάσεις των nearest neighbors(approximate και true)

        Destructor (~GNNS): Καθαρίζει τη μνήμη που έχει δεσμευθεί.

ΜΕΡΟΣ Β - MRNG