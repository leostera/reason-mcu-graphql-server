type superhero = {name: string};

type person = {
  first_name: string,
  last_name: string,
  hometown: string,
  alter_ego: option(superhero),
};

let people = [
  {
    first_name: "Tony",
    last_name: "Stark",
    hometown: "Manhattan",
    alter_ego: Some({name: "Iron Man"}),
  },
  {
    first_name: "Thor",
    last_name: "Odinsson",
    hometown: "Asgard",
    alter_ego: None,
  },
  {
    first_name: "Gamora",
    last_name: "Zen Whoberi Ben Titan",
    hometown: "Unknown Location",
    alter_ego: None,
  },
];

let alter_ego =
  Graphql.Schema.(
    obj(
      "Superhero",
      ~doc="A Superhero in the Marvel Cinematic Universe",
      ~fields=_ =>
      [
        field(
          "name",
          ~doc="The name of this superhero.",
          ~args=Arg.[],
          ~typ=non_null(string),
          ~resolve=((), p) =>
          p.name
        ),
      ]
    )
  );

let person =
  Graphql.Schema.(
    obj("Person", ~doc="A Person in the Marvel Cinematic Universe", ~fields=_ =>
      [
        field(
          "first_name",
          ~doc="The first name of this person.",
          ~args=Arg.[],
          ~typ=non_null(string),
          ~resolve=((), p) =>
          p.first_name
        ),
        field(
          "last_name",
          ~doc="The last name of this person.",
          ~args=Arg.[],
          ~typ=non_null(string),
          ~resolve=((), p) =>
          p.last_name
        ),
        field(
          "hometown",
          ~doc="Where they are from.",
          ~args=Arg.[],
          ~typ=non_null(string),
          ~resolve=((), p) =>
          p.hometown
        ),
        field(
          "alter_ego",
          ~doc="Their alternative identity.",
          ~args=Arg.[],
          ~typ=alter_ego,
          ~resolve=((), p) =>
          p.alter_ego
        ),
      ]
    )
  );

let schema =
  Graphql.Schema.(
    schema([
      field(
        "people",
        ~doc="All the people in the Marvel Cinematic Universe",
        ~args=Arg.[],
        ~typ=non_null(list(non_null(person))),
        ~resolve=((), ()) =>
        people
      ),
    ])
  );
