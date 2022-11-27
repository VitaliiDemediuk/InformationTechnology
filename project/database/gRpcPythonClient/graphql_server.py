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

from resolvers.table_mutation_resolver import CreateNewTableResolver
from resolvers.table_mutation_resolver import RenameTableResolver
from resolvers.table_mutation_resolver import DeleteTableResolver

from resolvers.column_mutation_resolver import CreateDefaultColumnResolver
from resolvers.column_mutation_resolver import CreateIntIntervalColumnResolver
from resolvers.column_mutation_resolver import RenameColumnResolver
from resolvers.column_mutation_resolver import DeleteColumnResolver


app = Flask(__name__)
CORS(app)

query = ObjectType("Query")
query.set_field("getTableList", GetTableListResolver())
query.set_field("getTableInfo", GetTableInfoResolver())
query.set_field("getTable", GetTableResolver())

mutation = ObjectType("Mutation")
mutation.set_field("createNewTable", CreateNewTableResolver())
mutation.set_field("renameTable", RenameTableResolver())
mutation.set_field("deleteTable", DeleteTableResolver())

mutation.set_field("createDefaultColumn", CreateDefaultColumnResolver())
mutation.set_field("createIntIntervalColumn", CreateIntIntervalColumnResolver())
mutation.set_field("renameColumn", RenameColumnResolver())
mutation.set_field("deleteColumn", DeleteColumnResolver())

type_defs = load_schema_from_path("schema.graphql")
schema = make_executable_schema(
    type_defs, query, mutation, snake_case_fallback_resolvers
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
