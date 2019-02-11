module BodyParser: {
  type error = [ | `ParseError(string)];
  let to_graphql:
    string =>
    result(
      (
        Graphql_parser.document,
        list((string, Graphql_parser.const_value)),
        string,
      ),
      error,
    );
} = {
  type error = [ | `ParseError(string)];

  open Yojson.Basic;

  let to_graphql = body_string => {
    switch (body_string |> from_string) {
    | json =>
      switch (
        json |> Util.member("query") |> Util.to_string,
        json |> Util.member("variables") |> Util.to_assoc,
        json |> Util.member("operationName") |> Util.to_string,
      ) {
      | (query, variables, operation_name) =>
        switch (Graphql_parser.parse(query)) {
        | Ok(doc) =>
          Ok((
            doc,
            (variables :> list((string, Graphql_parser.const_value))),
            operation_name,
          ))
        | Error(err) => Error(`ParseError(err))
        }
      | exception (Util.Type_error(err, t)) => Error(`ParseError(err))
      | exception _ =>
        Error(
          `ParseError(
            "Something went wrong while parsing the request body. A GraphQL request body usually contains a `query` string object, a `variables` JSON object and an `operationName` string.",
          ),
        )
      }
    | exception (Yojson.Json_error(err)) => Error(`ParseError(err))
    };
  };
};

module App = {
  type state = {version: string};
  let initial_state = {version: "1.0.0"};

  let route_handler: Httpkit.Server.Common.route_handler(state) =
    (ctx, path) =>
      switch (ctx.req.meth, path) {
      | (`POST, ["graphql"]) =>
        switch (ctx.body()) {
        | Some(body_string) =>
          switch (body_string |> BodyParser.to_graphql) {
          | Ok((doc, variables, operation_name)) =>
            /* Execute the Document with the Ctx and the Schema */
            switch (
              Graphql.Schema.execute(
                Schema.schema,
                (),
                doc,
                ~variables,
                ~operation_name,
              )
            ) {
            | Ok(`Response(data)) =>
              /* Serialize the Resulting Value */
              let json_str = data |> Yojson.Basic.to_string;
              /* Respond with serialized resulting value */
              `With_headers((
                200 |> Httpaf.Status.of_code,
                [
                  (
                    "Content-Length",
                    json_str |> String.length |> string_of_int,
                  ),
                ],
                json_str,
              ));
            | Ok(`Stream(_)) =>
              /* Ignore the stream for now */
              `With_status((
                400 |> Httpaf.Status.of_code,
                "Unsupported stream response",
              ))
            | Error(err) =>
              `With_status((
                400 |> Httpaf.Status.of_code,
                err |> Yojson.Basic.to_string,
              ))
            }
          | Error(`ParseError(err)) =>
            `With_status((
              400 |> Httpaf.Status.of_code,
              "Failed to parse request body: " ++ err,
            ))
          }
        | None =>
          `With_status((400 |> Httpaf.Status.of_code, "Missing request body"))
        }
      | _ => `Unmatched
      };
};

let make = (~port, ~on_start) => {
  Httpkit.Server.(
    make(App.initial_state)
    |> use(Common.log)
    |> reply(Common.router(App.route_handler))
    |> Httpkit_lwt.Server.Http.listen(~port, ~on_start)
    |> Lwt_main.run
  );
};