import Nav from "@/components /Nav"
import { useSession, signIn, signOut } from "next-auth/react"
import Layout from "@/components /layout"




export default function Home() {
  const {data: session} = useSession()
  
  return (
  <Layout>
    <div className="text-fuchsia-800 flex justify-between">
      <h2 className="text-fuchsia-800">
        Hello, <b>{session?.user?.name}</b>
      </h2>
      <div className="flex bg-fuchsia-800 gap-1 text-white rounded-lg overflow-hidden">
        <img src={session?.user?.image} alt="" className="w-6 h-6"/>
        <span className="px-2">
          {session?.user?.name}
        </span>
      </div>
    </div>
  </Layout>
  )

}
