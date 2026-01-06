# Détecteur de Cyberharcèlement en C v2.0

Projet de programmation assisté par IA - L2 MIASHS - Université Paris Nanterre

## Description

Ce projet implémente un détecteur de commentaires haineux en langage C avec une **dimension préventive unique**. Contrairement aux algorithmes des réseaux sociaux qui modèrent silencieusement, ce projet **éduque et prévient** en montrant pourquoi un message est problématique et en proposant des ressources d'aide.

### Ce qui nous différencie des réseaux sociaux :
- **Transparence** : On explique POURQUOI un message est toxique
- **Prévention pour les auteurs** : Avertissement AVANT publication
- **Aide aux victimes** : Liens vers des ressources (3018, netecoute.fr...)
- **Éducation** : Conseils pour reformuler constructivement

## Fonctionnalités v2.0

### Menu interactif (5 modes)
1. **Génération automatique** : L'IA génère des commentaires réalistes
2. **Analyser un commentaire reçu** : Pour les victimes qui veulent vérifier
3. **Vérifier avant de publier** : Pour les auteurs AVANT d'envoyer
4. **Mode conversation** : Analyse multiple avec statistiques
5. **Ressources d'aide** : Numéros et liens utiles

### Système de prévention
- Messages d'avertissement adaptés selon la gravité (score)
- Rappel des conséquences (signalement, plainte, poursuites)
- Proposition de reformulation
- Liens vers des ressources d'aide

## Ressources d'aide intégrées

| Ressource | Description |
|-----------|-------------|
| 3018 | Numéro national contre le cyberharcèlement |
| 3114 | Prévention du suicide (24h/24) |
| netecoute.fr | Écoute et conseils |
| e-enfance.org | Protection des mineurs |

## Prérequis

- Windows 11 avec MSYS2 MinGW-w64
- GCC 13.2.0 ou supérieur
- Bibliothèques : libcurl, json-c
- Clé API Mistral (gratuite sur https://console.mistral.ai/)

## Installation

### Sur Linux (Ubuntu/Debian)

```bash
# Installer les dépendances
sudo apt update
sudo apt install gcc make libcurl4-openssl-dev libjson-c-dev

# Cloner le projet
git clone https://github.com/amazing-source/algo-preventioncyberharcelement.git
cd cyberharcelement-detector

# Configurer la clé API (modifier config.h)
nano config.h
# Remplacer VOTRE_CLE_MISTRAL_ICI par votre clé

# Compiler
make

# Exécuter
./cyberharcelement_detector
```

### Sur Windows (MSYS2 MinGW64)

```bash
# Installer les dépendances
pacman -S mingw-w64-x86_64-gcc make
pacman -S mingw-w64-x86_64-curl
pacman -S mingw-w64-x86_64-json-c

# Compiler
make

# Exécuter
./cyberharcelement_detector
```

### Configuration de la clé API

1. Créer un compte sur https://console.mistral.ai/ (gratuit)
2. Générer une clé API
3. Modifier `config.h` :
```c
#define API_KEY "votre-cle-mistral-ici"
```

## Utilisation

```bash
# Lancer le programme (menu interactif)
./cyberharcelement_detector
```

### Menu Principal
```
[1] Generation automatique (IA)       - Tests automatiques
[2] Analyser un commentaire recu      - Pour les victimes
[3] Verifier avant de publier         - Pour les auteurs
[4] Mode conversation (multi-analyse) - Analyse en masse
[5] Ressources d'aide                 - Numeros et liens
[6] Quitter
```

## Structure du projet

```
.
├── main.c                  # Menu interactif et prévention
├── api_client.c/.h         # Communication avec l'API Mistral
├── comment_generator.c/.h  # Agent 1 : Génération réaliste
├── harassment_detector.c/.h # Agent 2 : Analyse stricte
├── stats.c/.h              # Calcul des statistiques
├── config.h                # Configuration API
├── Makefile
├── README.md
└── rapport/                # Fichiers source du rapport LaTeX
```

## Catégories de détection

| Catégorie | Description |
|-----------|-------------|
| sain | Critique normale, désaccord poli |
| rude | Remarques désagréables sans insultes |
| insulte | Insulte directe (con, idiot...) |
| humiliation | Attaque sur la personne |
| body-shaming | Attaques sur le physique (score min: 75) |
| discrimination | Racisme, sexisme, homophobie... |
| menace | Menace directe ou voilée |
| harcelement | Insultes répétées, acharnement |

## Interprétation des résultats

| Score | Niveau | Action suggérée |
|-------|--------|-----------------|
| 0-20 | Sain | OK |
| 20-40 | Rude | Reformuler conseillé |
| 40-60 | Insulte | Ne pas publier |
| 60-80 | Humiliation | Signalement possible |
| 80-100 | Grave | Ressources d'aide affichées |

## Auteurs

- **DJEMATENE Dilan** (44002835)
- **LACOMBE Ariane** (42006077)

## Licence

Projet académique - Université Paris Nanterre - 2025-2026
