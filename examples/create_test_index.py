from datetime import datetime
import elasticsearch5

es = elasticsearch5.Elasticsearch()
from elasticsearch5.client import IndicesClient
es_index = IndicesClient(es)
if es_index.exists("foo"):
    print ("foo already exists.");
else:
    es_index.create(index="foo", body=ur"""{
    "mappings": {
    "task": {
    "properties":{
        "owner":          {"type":"keyword"},
        "environment":    {"type":"keyword"},
        "ID":             {"type":"keyword"},
        "@timestamp":     {"type":"date", "format": "epoch_second"},
        "@process":       {"type":"integer"},
        "@db":            {"type":"integer"},
        "@wait":          {"type":"integer"},
        "status":         {"type":"keyword"}
      }}}}""")
