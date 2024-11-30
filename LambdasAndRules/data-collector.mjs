import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import { PutCommand, DynamoDBDocumentClient } from "@aws-sdk/lib-dynamodb";

const client = new DynamoDBClient({});
const docClient = DynamoDBDocumentClient.from(client);

console.log('Loading function');

export const handler = async (event, context) => {
    console.log(event);
    const command = new PutCommand({
        TableName: "data-temperaturas",
        Item: {
            entry_id: event.id,
            thing_name: event.thing_name,
            tempMLX: event.tempMLX,
            tempDHT: event.tempDHT
        }
    });

    const response = await docClient.send(command);
    console.log(response);
};