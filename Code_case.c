case 'a': 
      if(filename == NULL) {
        printw("Awa bay la vwa si dokiman aw épi opsyon -f ti misye \n");

      }
      if(ajout_fichier_au_zip(zip_file, optarg) !=0){
        printw("Non zanmi dokiman pa ka add kon sa\n");
        exit(EXIT_FAILURE);
      }
      printw("OOuuuiii fanmi la dokiman %s aw adan zip la \n!",optarg);
      exit(EXIT_SUCCESS);
      break;
