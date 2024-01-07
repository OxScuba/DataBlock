![Data Block](src/media/logo/DataBlock_logo.png)

Data Block

Bienvenue dans le projet Data Block, une application conçue pour les microcontrôleurs ESP32, en particulier le modèle Lilygo T-Display S3. Ce projet offre une interface conviviale pour afficher des informations en temps réel sur la Timechain Bitcoin, telles que la hauteur du bloc, les frais de transaction et bien plus encore.

Fonctionnalités

- Affichage des informations Bitcoin : Obtenez des données cruciales sur le réseau Bitcoin, notamment la hauteur du bloc et les frais de transaction actuels (faible, moyen, élevé).
- Exploration des derniers blocs : Explorez les détails des trois derniers blocs avec une interface inspirée de mempool.space.
- Compte à rebours personnalisable : Configurez des événements spéciaux avec un compte à rebours stylisé, comme illustré par l'exemple du BEF (18/05/2024).
- "Limit Fees" qui affiche 2 écrans différents en fonction de la limite choisis des fees moyen.

Configuration Wi-Fi

Au démarrage, l'ESP32 tente de se connecter automatiquement au dernier réseau Wi-Fi enregistré. Si aucun réseau n'est trouvé, il crée un point d'accès Wi-Fi auquel vous pouvez vous connecter pour configurer les paramètres Wi-Fi depuis votre téléphone.

![Data Block](src/media/exemples_displayScreens/1000x531_esp_config_wifi.png)

Une fois dans le portail de configuration Wi-Fi :
1) Cliquez sur "Configure WiFi" puis vous serez sur une nouvelle page
2) Ecrivez les informations concernant le Wi-Fi
3) Ainsi que la limite de Fees en sat/vB avant le Mordor
4) Cliquez sur "Save" pour sauvegarder la configuration
![Data Block](src/media/Portail_wifi/Portail_wifi.png)

Utilisation
Cliquez rapidement sur le bouton du bas afin d'affiché l'écran suivant. Une pression longue sur le bouton du bas vous ramènera au premier écran.
- Premier écran : Affiche les informations principales. L'écran comprend des détails tels que la hauteur du bloc et les frais de transaction actuels, ainsi qu'une image/logo sur la partie droite.
![Data Block](src/media/exemples_displayScreens/1000x531_esp_displayScreen1_exemple.png)

- Exploration des blocs : Explorez plus en détails les trois derniers blocs dans une interface inspirée de mempool.space (https://mempool.space/fr/)
![Data Block](src/media/exemples_displayScreens/1000x531_esp_mempool_exemples.png)

- Compte à rebours : Affiche un compte à rebours personnalisé pour des événements spéciaux.
![Data Block](src/media/exemples_displayScreens/1000x531_esp_events_countdown_exemple.png)

- Limit Fees : En fonction de la configuration dans le portail Wi-Fi, arrivez sur cet écrant, la comté ou le mordor sera affiché avec les fees moyen.
![Data Block](src/media/exemples_displayScreens/1000x531_esp_lotr_shire_fees_limit.png)
![Data Block](src/media/exemples_displayScreens/1000x531_esp_sauron_fees_limit.png)


Versions personnalisées

Le projet propose des versions personnalisées, telles que la version BEF et la version Seigneur des Anneaux (LOTR). Pour basculer entre ces versions, commentez le fichier "DataBlock.cpp" et décommentez la version de votre choix dans le dossier "src/editions".


Configuration

Pour démarrer, téléversez le firmware sur votre ESP32.

Note : Pour des versions plus personnalisées, consultez les fichiers correspondants dans le dossier "src/editions". 

Profitez de l'exploration de la Timechain Bitcoin avec Data Block ! 🚀
