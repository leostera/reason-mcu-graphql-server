open Lwt.Infix;

let handle_request = (reqd, _schema, _ctx) => {
  open Httpaf;
  let req = reqd |> Reqd.request;
  Logs.debug(m =>
    m("%s %s", req.meth |> Httpaf.Method.to_string, req.target)
  );
  switch (req.meth, req.target) {
  | (_, _) =>
    let res =
      Response.create(
        ~headers=Headers.of_list([("Content-Length", "0")]),
        `Not_implemented,
      );
    Httpaf.Reqd.respond_with_string(reqd, res, "");
    Lwt.return_unit;
  };
};

let connection_handler:
  (_, _, Unix.sockaddr, Lwt_unix.file_descr) => Lwt.t(unit) =
  (schema, ctx) =>
    Httpaf_lwt.Server.create_connection_handler(
      ~config=?None,
      ~request_handler=
        (_client, reqd) =>
          Lwt.async(() => handle_request(reqd, schema, ctx)),
      ~error_handler=
        (_client, ~request as _=?, error, start_response) => {
          open Httpaf;
          /** Originally from: https://github.com/anmonteiro/reason-graphql-experiment/blob/master/src/server/httpaf_server.re#L56-L118 */
          let response_body = start_response(Headers.empty);

          switch (error) {
          | `Exn(exn) =>
            Body.write_string(response_body, Printexc.to_string(exn));
            Body.write_string(response_body, "\n");
          | `Bad_gateway as error
          | `Bad_request as error
          | `Internal_server_error as error =>
            Body.write_string(
              response_body,
              Httpaf.Status.default_reason_phrase(error),
            )
          };

          Body.close_writer(response_body);
        },
    );

let start = (port, schema, ctx) => {
  let listening_address = Unix.ADDR_INET(Unix.inet_addr_loopback, port);

  /* Set up the http server */
  let _ =
    Lwt_io.establish_server_with_client_socket(
      listening_address,
      /* Pass in the schema and context so the response handlers */
      connection_handler(schema, ctx),
    )
    >>= (
      /* can use it */
      _ =>
        Logs_lwt.app(m => m("Server running at http://localhost:%d", port))
    );

  /* Keep the server running */
  let (forever, _) = Lwt.wait();
  forever |> Lwt_main.run;
};
