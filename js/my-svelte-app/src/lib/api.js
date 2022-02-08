import axios from "axios";

const api_url = API_BASE + '/graphql';

function GraphQLInit(context) {
	return async function (query, variables) {
		let cookie = context.get("cookie")
		let sid = cookie.sid || ""

		const data = {
			sid: sid,
			query: query,
			variables: variables,
		}

		const resp = await axios.post(api_url, data)

		if (resp.data.errors) {
			throw new Error(resp.data.errors[0].debugMessage || resp.data.errors[0].message);
		}

		if (resp.data.data) {
			return resp.data
		}

		throw new Error(resp.data);
	}
}

export { GraphQLInit }
