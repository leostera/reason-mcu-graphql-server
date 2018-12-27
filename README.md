# Marvel Cinematic Universe GraphQL Server

## Scope

We want to model People, Superheroes, Movies, and Superhero Groups.

#### Query for People [DONE]

A Person should have a first and last name, and hometown.

When we query for the list of people, we should see:

* Tony Stark, from Manhattan.
* Thor Odinsson, from Asgard.
* Gamora Zen Whoberi Ben Titan, from Unknown Location.

#### Query for a Person's alter ego [DONE]

A Person should have an alter_ego which wil be a Superhero.

When we query for the alter ego of all the people, we should see:

* Tony Stark as Iron Man.
