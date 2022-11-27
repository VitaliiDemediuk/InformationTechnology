# Flask
from flask import Flask
from flask_cors import CORS
from flask import request, jsonify

# ariadne
from ariadne import load_schema_from_path, make_executable_schema, \
    graphql_sync, snake_case_fallback_resolvers, ObjectType
from ariadne.constants import PLAYGROUND_HTML

# resolvers
from resolvers.get_table_list_resolver import GetTableListResolver
from resolvers.get_table_info_resolver import GetTableInfoResolver
from resolvers.get_table_resolver import GetTableResolver


app = Flask(__name__)
CORS(app)

query = ObjectType("Query")
query.set_field("getTableList", GetTableListResolver())
query.set_field("getTableInfo", GetTableInfoResolver())
query.set_field("getTable", GetTableResolver())

type_defs = load_schema_from_path("schema.graphql")
schema = make_executable_schema(
    type_defs, query, snake_case_fallback_resolvers
)


@app.route("/graphql", methods=["GET"])
def graphql_playground():
    return PLAYGROUND_HTML, 200


@app.route("/graphql", methods=["POST"])
def graphql_server():
    data = request.get_json()
    success, result = graphql_sync(
        schema,
        data,
        context_value=request,
        debug=app.debug
    )
    status_code = 200 if success else 400
    return jsonify(result), status_code